/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>

#include <morphuntion/dialog/ArticleDetectionFunction.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_CountLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_DefiniteArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_GenderLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_IndefiniteArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_ItDisplayFunction.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <unicode/uchar.h>

namespace morphuntion::grammar::synthesis {

void ItGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, u"", u"", u"il ", u"lo ", u"la ", u"l’", u"i ", u"gli ", u"le "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new ItGrammarSynthesizer_IndefiniteArticleLookupFunction(featureModel, u""));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_A_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, u"", u"a ", u"al ", u"allo ", u"alla ", u"all’", u"ai ", u"agli ", u"alle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_A_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, ARTICLE_A_PREPOSITION, u"a ", u"al ", u"allo ", u"alla ", u"all’", u"ai ", u"agli ", u"alle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DA_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, u"", u"da ", u"dal ", u"dallo ", u"dalla ", u"dall’", u"dai ", u"dagli ", u"dalle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DA_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, ARTICLE_DA_PREPOSITION, u"da ", u"dal ", u"dallo ", u"dalla ", u"dall’", u"dai ", u"dagli ", u"dalle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DE_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, u"", u"di ", u"del ", u"dello ", u"della ", u"dell’", u"dei ", u"degli ", u"delle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DE_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, ARTICLE_DE_PREPOSITION, u"di ", u"del ", u"dello ", u"della ", u"dell’", u"dei ", u"degli ", u"delle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_IN_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, u"", u"in ", u"nel ", u"nello ", u"nella ", u"nell’", u"nei ", u"negli ", u"nelle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_IN_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, ARTICLE_IN_PREPOSITION, u"in ", u"nel ", u"nello ", u"nella ", u"nell’", u"nei ", u"negli ", u"nelle "));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_SU_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, u"", u"su ", u"sul ", u"sullo ", u"sulla ", u"sull’", u"sui ", u"sugli ", u"sulle "));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_SU_PREPOSITION, new ItGrammarSynthesizer_DefiniteArticleLookupFunction(featureModel, ARTICLE_SU_PREPOSITION, u"su ", u"sul ", u"sullo ", u"sulla ", u"sull’", u"sui ", u"sugli ", u"sulle "));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ItGrammarSynthesizer_CountLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ItGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new morphuntion::dialog::ArticleDetectionFunction(morphuntion::util::LocaleUtils::ITALIAN(),
        {ARTICLE_DEFINITE, ARTICLE_DE_PREPOSITION, ARTICLE_A_PREPOSITION, ARTICLE_DA_PREPOSITION, ARTICLE_DE_PREPOSITION, ARTICLE_IN_PREPOSITION, ARTICLE_SU_PREPOSITION}, {u"a ", u"da ", u"de ", u"in ", u"su "},
        {ARTICLE_INDEFINITE}, {}));

    featureModel.setDefaultDisplayFunction(new ItGrammarSynthesizer_ItDisplayFunction(featureModel));


}

bool ItGrammarSynthesizer::startsWithConsonantSubset(const ::std::u16string& word)
{
    if (word.length() < 2) {
        return false;
    }
    auto firstChar = u_tolower(word[0]);
    auto secondChar = u_tolower(word[1]);
    return (firstChar == u'p' && (secondChar == u'n' || secondChar == u's')) || (firstChar == u'g' && secondChar == u'n') || firstChar == u'x' || firstChar == u'y' || firstChar == u'z' || (firstChar == u's' && !::morphuntion::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(secondChar));
}

bool ItGrammarSynthesizer::startsWithVowelForElision(const ::std::u16string& word)
{
    if (word.length() <= 1) {
        return false;
    }
    auto firstChar = u_tolower(word[0]);
    if (firstChar == u'i') {
        return !::morphuntion::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(word[1]);
    } else if (firstChar == u'h') {
        return ::morphuntion::dictionary::PhraseProperties::DEFAULT_VOWELS_START().contains(word[1]);
    }
    return ::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ITALIAN(), word);
}

ItGrammarSynthesizer::Count ItGrammarSynthesizer::getCount(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, ItGrammarSynthesizer::Count>({
        {GrammemeConstants::NUMBER_SINGULAR(), Count::singular},
        {GrammemeConstants::NUMBER_PLURAL(), Count::plural}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Count::undefined;
}

ItGrammarSynthesizer::Gender ItGrammarSynthesizer::getGender(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, ItGrammarSynthesizer::Gender>({
        {GrammemeConstants::GENDER_MASCULINE(), Gender::masculine},
        {GrammemeConstants::GENDER_FEMININE(), Gender::feminine}
    });
    if (value != nullptr) {
        auto result = npc(valueMap)->find(*npc(value));
        if (result != npc(valueMap)->end()) {
            return result->second;
        }
    }
    return Gender::undefined;
}

} // namespace morphuntion::grammar::synthesis
