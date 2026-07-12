/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>

#include <inflection/dialog/ArticleDetectionFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/GrammemeLookupFunction.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_DefiniteArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_IndefiniteArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_ItDisplayFunction.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/uchar.h>

namespace inflection::grammar::synthesis {

/*
 * It's OK to stop parsing at the first comma or period.
 * Assume that we're dealing decimal digits.
 */
static int64_t parseLeadingNumber(const ::std::u16string& str)
{
    int64_t num = -1;
    for (const auto ch : str) {
        if (!static_cast<bool>(u_isdigit(ch))) {
            break;
        }
        if (num < 0) {
            num = 0;
        }
        num = num * 10 + u_charDigitValue(ch);
    }
    return num;
}

static bool hasFeminineOrdinalSuffix(const ::std::u16string& str)
{
    for (auto ch : str) {
        if (!static_cast<bool>(u_isdigit(ch))) {
            return ch == u'ª';
        }
    }
    return false;
}

static bool hasOrdinalSuffix(const ::std::u16string& str)
{
    for (auto ch : str) {
        if (!static_cast<bool>(u_isdigit(ch))) {
            return ch == u'º' || ch == u'ª';
        }
    }
    return false;
}

static bool startsWithVowelForDigits(const ::std::u16string& str)
{
    auto num = parseLeadingNumber(str);
    if (num < 0) {
        return false;
    }
    while (num >= 1000000) {
        num /= 1000000;
    }
    if (num >= 1000) {
        if (num < 2000) {
            return false; // "mille" starts with 'm'
        }
        num /= 1000;
    }
    if (num >= 100) {
        if (num < 200) {
            return false; // "cento" starts with 'c'
        }
        num /= 100;
    }
    // uno, otto, undici, ottanta-ottantanove
    if (num == 1) {
        return !hasOrdinalSuffix(str);
    }
    return num == 8 || num == 11 || (num >= 80 && num <= 89);
}

void ItGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    auto numberLookup = new ::inflection::grammar::GrammemeLookupFunction(
        ::inflection::util::LocaleUtils::ITALIAN(),
        {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
        {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE, GrammemeConstants::POS_ARTICLE, GrammemeConstants::POS_VERB},
        u"", true,
        [](const ::std::u16string& word) -> ::std::u16string {
            if (!word.empty() && static_cast<bool>(u_isdigit(word[0]))) {
                return GrammemeConstants::NUMBER_SINGULAR;
            }
            if (word.ends_with(u"a") || word.ends_with(u"o")) {
                return GrammemeConstants::NUMBER_SINGULAR;
            }
            if (word.ends_with(u"i")) {
                return GrammemeConstants::NUMBER_PLURAL;
            }
            return {};
        });
    const auto& numberLookupRef = *npc(numberLookup);
    auto genderLookup = new ::inflection::grammar::GrammemeLookupFunction(
        ::inflection::util::LocaleUtils::ITALIAN(),
        {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE},
        {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE},
        u"", true,
        [](const ::std::u16string& word) -> ::std::u16string {
            if (!word.empty() && static_cast<bool>(u_isdigit(word[0]))) {
                if (hasFeminineOrdinalSuffix(word)) {
                    return GrammemeConstants::GENDER_FEMININE;
                }
                return GrammemeConstants::GENDER_MASCULINE;
            }
            if (word.ends_with(u"zione") || word.ends_with(u"sione") || word.ends_with(u"gione")
                || word.ends_with(u"si") || word.ends_with(u"à") || word.ends_with(u"ù")
                || word.ends_with(u"trice") || word.ends_with(u"tudine")) {
                return GrammemeConstants::GENDER_FEMININE;
            }
            if (word.ends_with(u"amma") || word.ends_with(u"ema") || word.ends_with(u"ore")
                || word.ends_with(u"è") || word.ends_with(u"ì") || word.ends_with(u"ò")
                || word.ends_with(u"i") || word.ends_with(u"o")) {
                return GrammemeConstants::GENDER_MASCULINE;
            }
            if (word.ends_with(u"a") || word.ends_with(u"e")) {
                return GrammemeConstants::GENDER_FEMININE;
            }
            return {};
        });
    const auto& genderLookupRef = *npc(genderLookup);

    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, nullptr, u"", u"il ", u"lo ", u"la ", u"l’", u"i ", u"gli ", u"le "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new ItGrammarSynthesizer_IndefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, nullptr));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_A_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, nullptr, u"a ", u"al ", u"allo ", u"alla ", u"all’", u"ai ", u"agli ", u"alle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_A_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, ARTICLE_A_PREPOSITION, u"a ", u"al ", u"allo ", u"alla ", u"all’", u"ai ", u"agli ", u"alle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DA_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, nullptr, u"da ", u"dal ", u"dallo ", u"dalla ", u"dall’", u"dai ", u"dagli ", u"dalle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DA_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, ARTICLE_DA_PREPOSITION, u"da ", u"dal ", u"dallo ", u"dalla ", u"dall’", u"dai ", u"dagli ", u"dalle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DE_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, nullptr, u"di ", u"del ", u"dello ", u"della ", u"dell’", u"dei ", u"degli ", u"delle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DE_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, ARTICLE_DE_PREPOSITION, u"di ", u"del ", u"dello ", u"della ", u"dell’", u"dei ", u"degli ", u"delle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_IN_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, nullptr, u"in ", u"nel ", u"nello ", u"nella ", u"nell’", u"nei ", u"negli ", u"nelle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_IN_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, ARTICLE_IN_PREPOSITION, u"in ", u"nel ", u"nello ", u"nella ", u"nell’", u"nei ", u"negli ", u"nelle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_SU_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, nullptr, u"su ", u"sul ", u"sullo ", u"sulla ", u"sull’", u"sui ", u"sugli ", u"sulle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_SU_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, numberLookupRef, genderLookupRef, ARTICLE_SU_PREPOSITION, u"su ", u"sul ", u"sullo ", u"sulla ", u"sull’", u"sui ", u"sugli ", u"sulle "));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, numberLookup);
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, genderLookup);
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new inflection::dialog::ArticleDetectionFunction(inflection::util::LocaleUtils::ITALIAN(),
        {ARTICLE_DEFINITE, ARTICLE_DE_PREPOSITION, ARTICLE_A_PREPOSITION, ARTICLE_DA_PREPOSITION, ARTICLE_DE_PREPOSITION, ARTICLE_IN_PREPOSITION, ARTICLE_SU_PREPOSITION}, {u"a ", u"da ", u"de ", u"in ", u"su "},
        {ARTICLE_INDEFINITE}, {}));

    featureModel.setDefaultDisplayFunction(new ItGrammarSynthesizer_ItDisplayFunction(featureModel));
}

bool ItGrammarSynthesizer::startsWithConsonantSubset(const ::std::u16string& word)
{
    if (word.length() < 2) {
        return false;
    }
    if (u_isdigit(word[0])) {
        return parseLeadingNumber(word) == 0;
    }
    auto firstChar = u_tolower(word[0]);
    auto secondChar = u_tolower(word[1]);
    return (firstChar == u'p' && (secondChar == u'n' || secondChar == u's')) || (firstChar == u'g' && secondChar == u'n') || firstChar == u'x' || firstChar == u'y' || firstChar == u'z' || (firstChar == u's' && !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(secondChar));
}

bool ItGrammarSynthesizer::startsWithVowelForElision(const ::std::u16string& word)
{
    if (word.empty()) {
        return false;
    }
    auto firstChar = u_tolower(word[0]);
    if (u_isdigit(firstChar)) {
        return startsWithVowelForDigits(word);
    }
    if (word.length() <= 1) {
        return false;
    }
    if (firstChar == u'i') {
        return !::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(word[1]);
    } else if (firstChar == u'h') {
        return ::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(word[1]);
    }
    return ::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ITALIAN(), word);
}

ItGrammarSynthesizer::Number ItGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR) {
            return Number::singular;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL) {
            return Number::plural;
        }
    }
    return Number::undefined;
}

ItGrammarSynthesizer::Gender ItGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE) {
            return Gender::masculine;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE) {
            return Gender::feminine;
        }
    }
    return Gender::undefined;
}

} // namespace inflection::grammar::synthesis
