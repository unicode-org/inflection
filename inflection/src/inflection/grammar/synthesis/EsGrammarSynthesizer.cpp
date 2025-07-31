/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EsGrammarSynthesizer.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_CountGenderLookupFunction.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_EsDisplayFunction.hpp>
#include <inflection/dialog/ArticleDetectionFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

void EsGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
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
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new inflection::dialog::ArticleDetectionFunction(inflection::util::LocaleUtils::SPANISH(),
        {ARTICLE_DEFINITE, ARTICLE_DE_PREPOSITION, ARTICLE_A_PREPOSITION}, {u"a", u"de"}, {ARTICLE_INDEFINITE}, {}));

    featureModel.setDefaultDisplayFunction(new EsGrammarSynthesizer_EsDisplayFunction(featureModel));
}

EsGrammarSynthesizer::Number EsGrammarSynthesizer::getNumber(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::NUMBER_SINGULAR()) {
            return Number::singular;
        }
        if (valRef == GrammemeConstants::NUMBER_PLURAL()) {
            return Number::plural;
        }
    }
    return Number::undefined;
}

EsGrammarSynthesizer::Gender EsGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_MASCULINE()) {
            return Gender::masculine;
        }
        if (valRef == GrammemeConstants::GENDER_FEMININE()) {
            return Gender::feminine;
        }
    }
    return Gender::undefined;
}

} // namespace inflection::grammar::synthesis
