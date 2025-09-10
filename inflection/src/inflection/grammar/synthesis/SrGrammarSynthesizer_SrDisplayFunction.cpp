/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#include <inflection/grammar/synthesis/SrGrammarSynthesizer_SrDisplayFunction.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <array>
#include <iterator>
#include <memory>
#include <string>

namespace inflection::grammar::synthesis {

SrGrammarSynthesizer_SrDisplayFunction::SrGrammarSynthesizer_SrDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::SERBIAN())))
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , numberFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , inflector(::inflection::dictionary::Inflector::getInflector(::inflection::util::LocaleUtils::SERBIAN()))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::SERBIAN()))
    , dictionaryInflector(::inflection::util::LocaleUtils::SERBIAN(),{
            {GrammemeConstants::POS_NOUN(), GrammemeConstants::POS_ADJECTIVE()},
            {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()},
            {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()}
    }, {}, true)
{
}

SrGrammarSynthesizer_SrDisplayFunction::~SrGrammarSynthesizer_SrDisplayFunction()
{
}

::std::u16string SrGrammarSynthesizer_SrDisplayFunction::inflectFromDictionary(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& lemma) const
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
    if (!caseString.empty() && caseString != GrammemeConstants::CASE_NOMINATIVE()) {
        string_constraints.emplace_back(caseString);
    }
    if (!genderString.empty()) {
        string_constraints.emplace_back(genderString);
    }
    int64_t wordGrammemes = 0;
    dictionary.getCombinedBinaryType(&wordGrammemes, lemma);

    auto inflectionResult = dictionaryInflector.inflect(lemma, wordGrammemes, string_constraints, {});
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
::std::u16string inflectByRuleOE(const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase, const ::std::u16string& gender);
// Neuter ending in e
::std::u16string inflectByRuleE(const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase, const ::std::u16string& gender);
// All genders ending in a
::std::u16string inflectByRuleA(const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase);
// Feminine, ending with consonant
::std::u16string inflectByRuleConsonant(const ::std::u16string& lemma, const ::std::u16string& number, const ::std::u16string& targetCase, const ::std::u16string& gender);

// Number of cases in Serbian.
static constexpr auto NUMBER_OF_CASES = 7UL;

// Given the table of all suffixes, both for singular and plural, append suffix to lemma, matching the number and case.
::std::u16string applySuffix(const ::std::u16string&, const ::std::array<::std::u16string_view, NUMBER_OF_CASES>&, const ::std::array<::std::u16string_view, NUMBER_OF_CASES>&, const ::std::u16string&, const ::std::u16string&);

// Check if proper noun by checking the first character is capital letter.
bool isProperNoun(const ::std::u16string &lemma);

} // namespace

::std::u16string SrGrammarSynthesizer_SrDisplayFunction::inflectWithRule(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, const ::std::u16string& lemma) const
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
        countString = GrammemeConstants::NUMBER_SINGULAR();
    }

    if (genderString.empty()) {
        genderString = GrammemeConstants::GENDER_MASCULINE();
    }

    // Do nothing for singular, nominative.
    if (countString == GrammemeConstants::NUMBER_SINGULAR() && caseString == GrammemeConstants::CASE_NOMINATIVE()) {
        return lemma;
    }

    // These are four declention groups in the language.
    if ((lemma.ends_with(u'о') || lemma.ends_with(u'е')) && (genderString == GrammemeConstants::GENDER_MASCULINE() || genderString == GrammemeConstants::GENDER_NEUTER())) {
        inflection = inflectByRuleOE(lemma, countString, caseString, genderString);
    } else if (lemma.ends_with(u'е') && genderString == GrammemeConstants::GENDER_NEUTER()) {
        inflection = inflectByRuleE(lemma, countString, caseString, genderString);
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

::inflection::dialog::DisplayValue *SrGrammarSynthesizer_SrDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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

static bool isConsonant(char16_t ch) {
    return ::inflection::lang::StringFilterUtil::CYRILLIC_SCRIPT().contains(ch) && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(ch);
}

static bool isVowel(char16_t ch) {
    return ::inflection::lang::StringFilterUtil::CYRILLIC_SCRIPT().contains(ch) && ::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(ch);
}

// Some rules require number of syllables in the word. It's counted as all vowels plus r if in between consonants, or if it starts a word followed by a consonant.
// We care about 1, 2 and more than 2 cases.
enum class Syllables {
    ONE_SYLLABLE,
    TWO_SYLLABLES,
    MULTI_SYLLABLES,
};
Syllables countSyllables(const ::std::u16string& lemma) {
    uint16_t total = 0;
    size_t index = 0;
    const size_t length = lemma.length();
    for (const char16_t ch: lemma) {
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

::std::u16string inflectByRuleOE(const ::std::u16string &lemma, [[maybe_unused]] const ::std::u16string &number, [[maybe_unused]] const ::std::u16string &targetCase, [[maybe_unused]] const ::std::u16string &gender)
{
    // TODO(nciric): implement logic.
    return lemma;
}

::std::u16string inflectByRuleE(const ::std::u16string &lemma, [[maybe_unused]] const ::std::u16string &number, [[maybe_unused]] const ::std::u16string &targetCase, [[maybe_unused]] const ::std::u16string &gender)
{
    // TODO(nciric): implement logic.
    return lemma;
}

::std::u16string inflectByRuleA(const ::std::u16string &lemma, const ::std::u16string &number, const ::std::u16string &targetCase)
{
    static constexpr auto suffix_sg = ::std::to_array<::std::u16string_view>({u"а", u"е", u"и", u"у", u"а", u"ом", u"и"});
    static constexpr auto suffix_pl = ::std::to_array<::std::u16string_view>({u"е", u"а", u"ама", u"е", u"е", u"ама", u"ама"});

    ::std::u16string base = lemma;
    // Remove trailing a and apply suffix.
    base.pop_back();
    base = applySuffix(base, suffix_sg, suffix_pl, number, targetCase);

    // Vocative singular and genitive plural require special processing in some cases.
    if (number == GrammemeConstants::NUMBER_SINGULAR() && targetCase == GrammemeConstants::CASE_VOCATIVE()) {
        Syllables syllables = countSyllables(lemma);
        if (lemma.ends_with(u"ица") && syllables == Syllables::MULTI_SYLLABLES) {
            base.back() = u'е';
        }
        if (isProperNoun(lemma) && syllables == Syllables::TWO_SYLLABLES) {
            base.back() = u'о';
        }
    }

    if (number == GrammemeConstants::NUMBER_PLURAL() && targetCase == GrammemeConstants::CASE_GENITIVE()) {
        if (lemma.ends_with(u"тња") || lemma.ends_with(u"дња") || lemma.ends_with(u"пта") || lemma.ends_with(u"лба") || lemma.ends_with(u"рва")) {
            base.back() = u'и';
        }
        static const char16_t *mappings[][2] = {
            {u"јка", u"јака"},
            {u"мља", u"маља"},
            {u"вца", u"ваца"},
            {u"тка", u"така"},
            {u"пка", u"пака"},
        };
        for (const auto &[suffix, replacement] : mappings) {
            if (base.ends_with(suffix)) {
                auto suffix_length = std::u16string_view(suffix).length();
                base.replace(base.length() - suffix_length, suffix_length, replacement);
            }
        }
    }

    return base;
}

::std::u16string inflectByRuleConsonant(const ::std::u16string &lemma, [[maybe_unused]] const ::std::u16string &number, [[maybe_unused]] const ::std::u16string &targetCase, [[maybe_unused]] const ::std::u16string & gender)
{
    // TODO(nciric): implement logic.
    return lemma;
}

::std::u16string applySuffix(const ::std::u16string &lemma, const ::std::array<::std::u16string_view, NUMBER_OF_CASES>& suffix_sg, const ::std::array<::std::u16string_view, NUMBER_OF_CASES>& suffix_pl,
    const ::std::u16string &number, const ::std::u16string &targetCase)
{
    const ::std::map<::std::u16string, size_t> case_index = {
        {GrammemeConstants::CASE_NOMINATIVE(), 0},
        {GrammemeConstants::CASE_GENITIVE(), 1},
        {GrammemeConstants::CASE_DATIVE(), 2},
        {GrammemeConstants::CASE_ACCUSATIVE(), 3},
        {GrammemeConstants::CASE_VOCATIVE(), 4},
        {GrammemeConstants::CASE_INSTRUMENTAL(), 5},
        {GrammemeConstants::CASE_LOCATIVE(), 6}
    };

    auto index = case_index.at(targetCase);

    if (number == GrammemeConstants::NUMBER_SINGULAR()) {
        return lemma + ::std::u16string(suffix_sg[index]);
    } else {
        return lemma + ::std::u16string(suffix_pl[index]);
    }
}

bool isProperNoun(const ::std::u16string &lemma) {
    // Check if first character is in range of Cyrl capital letters.
    auto first_ch = lemma.front();
    if (0x402 <= first_ch && first_ch <= 0x428) {
        return true;
    }

    return false;
}

} // namespace

} // namespace inflection::grammar::synthesis
