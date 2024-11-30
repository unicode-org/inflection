/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer_CountGenderLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer_EsDisplayFunction.hpp>
#include <morphuntion/dialog/ArticleDetectionFunction.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

void EsGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr, u"", u"el", u"la", u"los", u"las", true));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr, u"", u"un", u"una", u"unos", u"unas", true));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DE_PREPOSITION, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr, u"", u"del", u"de la", u"de los", u"de las", true));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DE_PREPOSITION, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, ARTICLE_DE_PREPOSITION, u"", u"del", u"de la", u"de los", u"de las", true));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_A_PREPOSITION, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr, u"", u"al", u"a la", u"a los", u"a las", true));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_A_PREPOSITION, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, ARTICLE_A_PREPOSITION, u"", u"al", u"a la", u"a los", u"a las", true));
    featureModel.putDefaultFeatureFunctionByName(DEMONSTRATIVE_ADJECTIVE, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, nullptr, u"", u"este", u"esta", u"estos", u"estas"));
    featureModel.putDefaultFeatureFunctionByName(WITH_DEMONSTRATIVE_ADJECTIVE, new EsGrammarSynthesizer_ArticleLookupFunction(featureModel, DEMONSTRATIVE_ADJECTIVE, u"", u"este", u"esta", u"estos", u"estas"));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new EsGrammarSynthesizer_CountGenderLookupFunction(EsGrammarSynthesizer_CountGenderLookupFunction::NUMBER_CATEGORY, {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new EsGrammarSynthesizer_CountGenderLookupFunction(EsGrammarSynthesizer_CountGenderLookupFunction::GENDER_CATEGORY, {GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_MASCULINE()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new morphuntion::dialog::ArticleDetectionFunction(morphuntion::util::LocaleUtils::SPANISH(),
        {ARTICLE_DEFINITE, ARTICLE_DE_PREPOSITION, ARTICLE_A_PREPOSITION}, {u"a", u"de"}, {ARTICLE_INDEFINITE}, {}));

    featureModel.setDefaultDisplayFunction(new EsGrammarSynthesizer_EsDisplayFunction(featureModel));
}

EsGrammarSynthesizer::Count EsGrammarSynthesizer::getCount(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, EsGrammarSynthesizer::Count>({
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

EsGrammarSynthesizer::Gender EsGrammarSynthesizer::getGender(const ::std::u16string* value) {
    static auto valueMap = new ::std::map<::std::u16string, EsGrammarSynthesizer::Gender>({
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
