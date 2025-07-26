/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/SvGrammarSynthesizer.hpp>

#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_SvDisplayFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

void SvGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(DEF_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"den", u"det", u"de"));
    featureModel.putDefaultFeatureFunctionByName(INDEF_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"en", u"ett", u"flera"));
    featureModel.putDefaultFeatureFunctionByName(INDEFINITE_PRONOUN, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"någon", u"något", u"några"));
    featureModel.putDefaultFeatureFunctionByName(WITH_INDEFINITE_PRONOUN, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"någon", u"något", u"några"));
    featureModel.putDefaultFeatureFunctionByName(INTERROGATIVE_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"vilken", u"vilket", u"vilka"));
    featureModel.putDefaultFeatureFunctionByName(WITH_INTERROGATIVE_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"vilken", u"vilket", u"vilka"));
    featureModel.putDefaultFeatureFunctionByName(NEG_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ingen", u"inget", u"inga"));
    featureModel.putDefaultFeatureFunctionByName(WITH_NEG_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ingen", u"inget", u"inga"));
    featureModel.putDefaultFeatureFunctionByName(NEW_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ny", u"nytt", u"nya"));
    featureModel.putDefaultFeatureFunctionByName(WITH_NEW_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ny", u"nytt", u"nya"));
    featureModel.putDefaultFeatureFunctionByName(OTHER_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"annan", u"annat", u"andra"));
    featureModel.putDefaultFeatureFunctionByName(WITH_OTHER_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"annan", u"annat", u"andra"));
    featureModel.putDefaultFeatureFunctionByName(POSS_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"din", u"ditt", u"dina"));
    featureModel.putDefaultFeatureFunctionByName(WITH_POSS_ARTICLE, new SvGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"din", u"ditt", u"dina"));
    featureModel.setDefaultDisplayFunction(new SvGrammarSynthesizer_SvDisplayFunction(featureModel));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::SWEDISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new SvGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::SWEDISH(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE()}));
}

SvGrammarSynthesizer::Number SvGrammarSynthesizer::getNumber(const ::std::u16string* value) {
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

SvGrammarSynthesizer::Gender SvGrammarSynthesizer::getGender(const ::std::u16string* value) {
    if (value != nullptr) {
        const auto& valRef = *value;
        if (valRef == GrammemeConstants::GENDER_COMMON()) {
            return Gender::common;
        }
        if (valRef == GrammemeConstants::GENDER_NEUTER()) {
            return Gender::neuter;
        }
    }
    return Gender::undefined;
}

} // namespace inflection::grammar::synthesis
