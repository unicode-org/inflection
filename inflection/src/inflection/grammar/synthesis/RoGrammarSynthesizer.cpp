/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/RoGrammarSynthesizer.hpp>

#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/PrefixedDisplayFunction.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void RoGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ROMANIAN(), {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ROMANIAN(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ROMANIAN(), {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_DATIVE, GrammemeConstants::CASE_ACCUSATIVE, GrammemeConstants::CASE_INSTRUMENTAL, GrammemeConstants::CASE_LOCATIVE, GrammemeConstants::CASE_VOCATIVE}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::ROMANIAN(), {GrammemeConstants::DEFINITENESS_DEFINITE, GrammemeConstants::DEFINITENESS_INDEFINITE}));
    featureModel.setDefaultDisplayFunction(new ::inflection::grammar::PrefixedDisplayFunction(featureModel, ::inflection::util::LocaleUtils::ROMANIAN(), {
        {GrammemeConstants::POS_NOUN, GrammemeConstants::POS_ADJECTIVE},
        {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE},
        {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
        {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}
    }, GrammemeConstants::DEFINITENESS));
}

} // namespace inflection::grammar::synthesis
