/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/DaGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/DaGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/DaGrammarSynthesizer_DaDisplayFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

void DaGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_NEGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ingen", u"intet", u"ingen"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_NEGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ingen", u"intet", u"ingen"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_DEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, ::inflection::dialog::SpeakableString(u"den", u"dén"), ::inflection::dialog::SpeakableString(u"det"), ::inflection::dialog::SpeakableString(u"de")));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_DEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, ::inflection::dialog::SpeakableString(u"den", u"dén"), ::inflection::dialog::SpeakableString(u"det"), ::inflection::dialog::SpeakableString(u"de")));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"en", u"et", u"flere"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"en", u"et", u"flere"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_POSSESSIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"din", u"dit", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_POSSESSIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"din", u"dit", u"dine"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_NEW, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"ny", u"nyt", u"nye"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_NEW, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"ny", u"nyt", u"nye"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_OTHER, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"anden", u"andet", u"andre"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_OTHER, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"anden", u"andet", u"andre"));
    featureModel.putDefaultFeatureFunctionByName(ARTICLE_INTERROGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(WITH_ARTICLE_INTERROGATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"hvilken", u"hvilket", u"hvilke"));
    featureModel.putDefaultFeatureFunctionByName(PRONOUN_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"nogen", u"noget", u"nogle"));
    featureModel.putDefaultFeatureFunctionByName(WITH_PRONOUN_INDEFINITE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"nogen", u"noget", u"nogle"));
    featureModel.putDefaultFeatureFunctionByName(PRONOUN_DEMONSTRATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, false, u"denne", u"dette", u"disse"));
    featureModel.putDefaultFeatureFunctionByName(WITH_PRONOUN_DEMONSTRATIVE, new DaGrammarSynthesizer_ArticleLookupFunction(featureModel, true, u"denne", u"dette", u"disse"));
    featureModel.setDefaultDisplayFunction(new DaGrammarSynthesizer_DaDisplayFunction(featureModel));

    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::DANISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::DANISH(), {GrammemeConstants::GENDER_COMMON(), GrammemeConstants::GENDER_NEUTER()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::DANISH(), {GrammemeConstants::DEFINITENESS_DEFINITE(), GrammemeConstants::DEFINITENESS_INDEFINITE()}));
}

DaGrammarSynthesizer::Number DaGrammarSynthesizer::getNumber(const ::std::u16string* value) {
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

DaGrammarSynthesizer::Gender DaGrammarSynthesizer::getGender(const ::std::u16string* value) {
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
