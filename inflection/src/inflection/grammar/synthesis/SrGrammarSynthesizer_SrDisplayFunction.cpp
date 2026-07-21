/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#include <inflection/grammar/synthesis/SrGrammarSynthesizer_SrDisplayFunction.hpp>

#include <array>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <unicode/uscript.h>

namespace inflection::grammar::synthesis {

SrGrammarSynthesizer_SrDisplayFunction::SrGrammarSynthesizer_SrDisplayFunction(
    const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(
          ::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::SERBIAN())))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::SERBIAN()))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::SERBIAN()))
    , dictionaryInflector(::inflection::util::LocaleUtils::SERBIAN(),
          { { GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE },
              { GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL },
              { GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE,
                  GrammemeConstants::GENDER_NEUTER } },
          { }, true)
{
}

SrGrammarSynthesizer_SrDisplayFunction::~SrGrammarSynthesizer_SrDisplayFunction() { }

::std::u16string SrGrammarSynthesizer_SrDisplayFunction::inflectFromDictionary(
    const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
    const ::std::u16string& lemma) const
{
    ::std::u16string countString(GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature));
    ::std::u16string caseString(GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature));
    auto genderString = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature);

    ::std::u16string inflection;

    // The nominative/caseless is unmarked in the patterns so we need to do something like this
    ::std::vector<::std::u16string> string_constraints;
    if (!countString.empty()) {
        string_constraints.emplace_back(countString);
    }
    if (!caseString.empty() && caseString != GrammemeConstants::CASE_NOMINATIVE) {
        string_constraints.emplace_back(caseString);
    }
    if (!genderString.empty()) {
        string_constraints.emplace_back(genderString);
    }
    int64_t wordGrammemes = 0;
    dictionary.getCombinedBinaryType(&wordGrammemes, lemma);

    auto inflectionResult = dictionaryInflector.inflect(lemma, wordGrammemes, string_constraints, { });
    if (inflectionResult) {
        inflection = *inflectionResult;
    }

    if (inflection.empty()) {
        return lemma;
    }

    return inflection;
}

namespace {

// Rule based inflectors for four declination groups.
// Masculine or neuter ending in o or e and masculine ending with consonant.
::std::u16string inflectByRuleOE(const ::std::u16string& lemma, const ::std::u16string& number,
    const ::std::u16string& targetCase, const ::std::u16string& gender);
// Neuter ending in e
::std::u16string inflectByRuleE(const ::std::u16string& lemma, const ::std::u16string& number,
    const ::std::u16string& targetCase, const ::std::u16string& gender);
// All genders ending in a
::std::u16string inflectByRuleA(
    const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase);
// Feminine, ending with consonant
::std::u16string inflectByRuleConsonant(const ::std::u16string& lemma, const ::std::u16string& number,
    const ::std::u16string& targetCase, const ::std::u16string& gender);

// Number of cases in Serbian.
static constexpr auto NUMBER_OF_CASES = 7UL;

// Given the table of all suffixes, both for singular and plural, append suffix to lemma, matching the number and
// case.
::std::u16string applySuffix(const ::std::u16string&, const ::std::array<::std::u16string_view, NUMBER_OF_CASES>&,
    const ::std::array<::std::u16string_view, NUMBER_OF_CASES>&, const ::std::u16string&, const ::std::u16string&);

// Check if proper noun by checking the first character is capital letter.
bool isProperNoun(const ::std::u16string& lemma);

} // namespace

::std::u16string SrGrammarSynthesizer_SrDisplayFunction::inflectWithRule(
    const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
    const ::std::u16string& lemma) const
{
    auto countString = GrammarSynthesizerUtil::getFeatureValue(constraints, numberFeature);
    auto caseString = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature);
    auto genderString = GrammarSynthesizerUtil::getFeatureValue(constraints, genderFeature);

    ::std::u16string inflection;

    if (caseString.empty()) {
        return lemma;
    }

    // Set defaults for number and gender if missing.
    if (countString.empty()) {
        countString = GrammemeConstants::NUMBER_SINGULAR;
    }

    if (genderString.empty()) {
        genderString = GrammemeConstants::GENDER_MASCULINE;
    }

    // Do nothing for singular, nominative.
    if (countString == GrammemeConstants::NUMBER_SINGULAR && caseString == GrammemeConstants::CASE_NOMINATIVE) {
        return lemma;
    }

    // These are four declension groups in the language.
    if ((lemma.ends_with(u"ме") || (lemma.ends_with(u"те") && !lemma.ends_with(u"ште")) || lemma.ends_with(u"же")
            || lemma.ends_with(u"ле") || lemma.ends_with(u"че") || lemma.ends_with(u"бе"))
        && genderString == GrammemeConstants::GENDER_NEUTER) {
        inflection = inflectByRuleE(lemma, countString, caseString, genderString);
    } else if ((lemma.ends_with(u'о') || lemma.ends_with(u'е'))
        && (genderString == GrammemeConstants::GENDER_MASCULINE || genderString == GrammemeConstants::GENDER_NEUTER)) {
        inflection = inflectByRuleOE(lemma, countString, caseString, genderString);
    } else if (lemma.ends_with(u'а')) {
        inflection = inflectByRuleA(lemma, countString, caseString);
    } else {
        inflection = inflectByRuleConsonant(lemma, countString, caseString, genderString);
    }

    if (inflection.empty()) {
        inflection = lemma;
    }

    return inflection;
}

::inflection::dialog::DisplayValue* SrGrammarSynthesizer_SrDisplayFunction::getDisplayValue(
    const dialog::SemanticFeatureModel_DisplayData& displayData,
    const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
    bool enableInflectionGuess) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    if (displayString.empty()) {
        return nullptr;
    }
    if (dictionary.isKnownWord(displayString)) {
        displayString = inflectFromDictionary(constraints, displayString);
    } else if (enableInflectionGuess) {
        // Let's use rule based inflection for nouns. Assume lemma is singular, nominative.
        displayString = inflectWithRule(constraints, displayString);
    }
    return new ::inflection::dialog::DisplayValue(displayString, constraints);
}

namespace {

static bool isConsonant(char16_t ch)
{
    return uscript_hasScript(ch, USCRIPT_CYRILLIC)
        && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(ch);
}

static bool isVowel(char16_t ch)
{
    return uscript_hasScript(ch, USCRIPT_CYRILLIC)
        && ::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(ch);
}

// Some rules require number of syllables in the word. It's counted as all vowels plus r if in between consonants,
// or if it starts a word followed by a consonant. We care about 1, 2 and more than 2 cases.
enum class Syllables {
    ONE_SYLLABLE,
    TWO_SYLLABLES,
    MULTI_SYLLABLES,
};
Syllables countSyllables(const ::std::u16string& lemma)
{
    uint16_t total = 0;
    size_t index = 0;
    const size_t length = lemma.length();
    for (const char16_t ch : lemma) {
        if (isVowel(ch)) {
            ++total;
        }
        // Check case where R is at the begining followed by a consonant.
        if ((ch == u'р' || ch == u'Р') && (index == 0 && index + 1 < length)) {
            if (isConsonant(lemma[index + 1])) {
                ++total;
            }
        } else if ((ch == u'р' || ch == u'Р') && (index != 0 && index + 1 < length)) {
            if (isConsonant(lemma[index - 1]) && isConsonant(lemma[index + 1])) {
                ++total;
            }
        }
        ++index;
    }

    if (total == 1) {
        return Syllables::ONE_SYLLABLE;
    } else if (total == 2) {
        return Syllables::TWO_SYLLABLES;
    } else {
        return Syllables::MULTI_SYLLABLES;
    }
}

constexpr size_t getCaseIndex(std::u16string_view targetCase)
{
    if (targetCase == GrammemeConstants::CASE_NOMINATIVE) {
        return 0;
    }
    if (targetCase == GrammemeConstants::CASE_GENITIVE) {
        return 1;
    }
    if (targetCase == GrammemeConstants::CASE_DATIVE) {
        return 2;
    }
    if (targetCase == GrammemeConstants::CASE_ACCUSATIVE) {
        return 3;
    }
    if (targetCase == GrammemeConstants::CASE_VOCATIVE) {
        return 4;
    }
    if (targetCase == GrammemeConstants::CASE_INSTRUMENTAL) {
        return 5;
    }
    if (targetCase == GrammemeConstants::CASE_LOCATIVE) {
        return 6;
    }
    return 0;
}

static bool isSoftStemChar(char16_t ch)
{
    return ch == u'ж' || ch == u'ч' || ch == u'ш' || ch == u'ћ' || ch == u'ђ' || ch == u'љ' || ch == u'њ' || ch == u'ј'
        || ch == u'ц';
}

static bool isSoftStem(std::u16string_view stem)
{
    if (stem.empty()) {
        return false;
    }
    if (stem.ends_with(u"шт")) {
        return true;
    }
    return isSoftStemChar(stem.back());
}

::std::u16string applySuffix(const ::std::u16string& lemma,
    const ::std::array<::std::u16string_view, NUMBER_OF_CASES>& suffix_sg,
    const ::std::array<::std::u16string_view, NUMBER_OF_CASES>& suffix_pl, const ::std::u16string& number,
    const ::std::u16string& targetCase)
{
    auto index = getCaseIndex(targetCase);

    if (number == GrammemeConstants::NUMBER_SINGULAR) {
        return lemma + ::std::u16string(suffix_sg[index]);
    } else {
        return lemma + ::std::u16string(suffix_pl[index]);
    }
}

::std::u16string inflectByRuleOE(const ::std::u16string& lemma, const ::std::u16string& number,
    const ::std::u16string& targetCase, [[maybe_unused]] const ::std::u16string& gender)
{
    if (lemma.empty()) {
        return lemma;
    }

    ::std::u16string base = lemma;
    char16_t ending = base.back();
    base.pop_back();

    bool soft = isSoftStem(base);

    std::array<::std::u16string_view, NUMBER_OF_CASES> suffix_sg;
    if (ending == u'е') {
        suffix_sg
            = ::std::to_array<::std::u16string_view>({ u"е", u"а", u"у", u"е", u"е", soft ? u"ем" : u"ом", u"у" });
    } else {
        suffix_sg
            = ::std::to_array<::std::u16string_view>({ u"о", u"а", u"у", u"о", u"о", soft ? u"ем" : u"ом", u"у" });
    }
    static constexpr auto suffix_pl
        = ::std::to_array<::std::u16string_view>({ u"а", u"а", u"има", u"а", u"а", u"има", u"има" });

    return applySuffix(base, suffix_sg, suffix_pl, number, targetCase);
}

::std::u16string inflectByRuleE(const ::std::u16string& lemma, const ::std::u16string& number,
    const ::std::u16string& targetCase, [[maybe_unused]] const ::std::u16string& gender)
{
    if (lemma.empty()) {
        return lemma;
    }

    ::std::u16string base = lemma;
    base.pop_back();

    if (lemma.ends_with(u"ме")) {
        base += u"ен";
    } else if ((lemma.ends_with(u"те") && !lemma.ends_with(u"ште")) || lemma.ends_with(u"же") || lemma.ends_with(u"ле")
        || lemma.ends_with(u"че") || lemma.ends_with(u"бе")) {
        base += u"ет";
    }

    static constexpr auto suffix_sg
        = ::std::to_array<::std::u16string_view>({ u"е", u"а", u"у", u"е", u"е", u"ом", u"у" });
    static constexpr auto suffix_pl
        = ::std::to_array<::std::u16string_view>({ u"а", u"а", u"има", u"а", u"а", u"има", u"има" });

    return applySuffix(base, suffix_sg, suffix_pl, number, targetCase);
}

::std::u16string inflectByRuleA(
    const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase)
{
    static constexpr auto suffix_sg
        = ::std::to_array<::std::u16string_view>({ u"а", u"е", u"и", u"у", u"а", u"ом", u"и" });
    static constexpr auto suffix_pl
        = ::std::to_array<::std::u16string_view>({ u"е", u"а", u"ама", u"е", u"е", u"ама", u"ама" });

    ::std::u16string base = lemma;
    // Remove trailing a and apply suffix.
    base.pop_back();
    base = applySuffix(base, suffix_sg, suffix_pl, number, targetCase);

    // Vocative singular and genitive plural require special processing in some cases.
    if (number == GrammemeConstants::NUMBER_SINGULAR && targetCase == GrammemeConstants::CASE_VOCATIVE) {
        Syllables syllables = countSyllables(lemma);
        if (lemma.ends_with(u"ица") && syllables == Syllables::MULTI_SYLLABLES) {
            base.back() = u'е';
        } else if (isProperNoun(lemma) && syllables == Syllables::TWO_SYLLABLES) {
            base.back() = u'о';
        }
    }

    if (number == GrammemeConstants::NUMBER_PLURAL && targetCase == GrammemeConstants::CASE_GENITIVE) {
        if (lemma.ends_with(u"тња") || lemma.ends_with(u"дња") || lemma.ends_with(u"пта") || lemma.ends_with(u"лба")
            || lemma.ends_with(u"рва")) {
            base.back() = u'и';
        }
        static const char16_t* mappings[][2] = {
            { u"јка", u"јака" },
            { u"мља", u"маља" },
            { u"вца", u"ваца" },
            { u"тка", u"така" },
            { u"пка", u"пака" },
        };
        for (const auto& [suffix, replacement] : mappings) {
            if (base.ends_with(suffix)) {
                auto suffix_length = std::u16string_view(suffix).length();
                base.replace(base.length() - suffix_length, suffix_length, replacement);
            }
        }
    }

    return base;
}

::std::u16string inflectByRuleFeminineConsonant(
    const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase)
{
    ::std::u16string base = lemma;
    if (number == GrammemeConstants::NUMBER_SINGULAR && targetCase == GrammemeConstants::CASE_INSTRUMENTAL) {
        if (base.ends_with(u"в") || base.ends_with(u"б") || base.ends_with(u"м") || base.ends_with(u"п")) {
            return base + u"љу";
        }
        if (base.ends_with(u"ст")) {
            base.replace(base.length() - 2, 2, u"шћ");
            return base + u"у";
        }
        if (base.ends_with(u"т")) {
            base.back() = u'ћ';
            return base + u"у";
        }
        if (base.ends_with(u"ћ") || base.ends_with(u"ч") || base.ends_with(u"ш") || base.ends_with(u"ж")
            || base.ends_with(u"ђ") || base.ends_with(u"љ") || base.ends_with(u"њ")) {
            return base + u"у";
        }
        if (base.ends_with(u"р")) {
            return base + u"ју";
        }
    }
    static constexpr auto suffix_sg_fem
        = ::std::to_array<::std::u16string_view>({ u"", u"и", u"и", u"", u"и", u"и", u"и" });
    static constexpr auto suffix_pl_fem
        = ::std::to_array<::std::u16string_view>({ u"и", u"и", u"има", u"и", u"и", u"има", u"има" });
    return applySuffix(base, suffix_sg_fem, suffix_pl_fem, number, targetCase);
}

::std::u16string inflectByRuleMasculineConsonant(
    const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase)
{
    ::std::u16string base = lemma;
    // Handle fleeting -a- (ац -> ц, нак -> нк, лак -> лк)
    if ((lemma.ends_with(u"ац") || lemma.ends_with(u"нак") || lemma.ends_with(u"лак") || lemma.ends_with(u"цак"))
        && lemma.length() > 2) {
        base.erase(base.length() - 2, 1);
    }

    bool soft = isSoftStem(base);

    std::array<::std::u16string_view, NUMBER_OF_CASES> suffix_sg
        = { u"", u"а", u"у", u"а", soft ? u"у" : u"е", soft ? u"ем" : u"ом", u"у" };
    static constexpr auto suffix_pl
        = ::std::to_array<::std::u16string_view>({ u"и", u"а", u"има", u"е", u"и", u"има", u"има" });

    return applySuffix(base, suffix_sg, suffix_pl, number, targetCase);
}

::std::u16string inflectByRuleConsonant(const ::std::u16string& lemma, const ::std::u16string& number,
    const ::std::u16string& targetCase, const ::std::u16string& gender)
{
    if (lemma.empty()) {
        return lemma;
    }

    if (number == GrammemeConstants::NUMBER_SINGULAR && targetCase == GrammemeConstants::CASE_NOMINATIVE) {
        return lemma;
    }

    if (gender == GrammemeConstants::GENDER_FEMININE) {
        return inflectByRuleFeminineConsonant(lemma, number, targetCase);
    }

    return inflectByRuleMasculineConsonant(lemma, number, targetCase);
}

bool isProperNoun(const ::std::u16string& lemma)
{
    // Check if first character is in range of Cyrl capital letters.
    auto first_ch = lemma.front();
    if (0x402 <= first_ch && first_ch <= 0x428) {
        return true;
    }

    return false;
}

} // namespace

} // namespace inflection::grammar::synthesis
