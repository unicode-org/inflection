/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/KnGrammarSynthesizer.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/GrammemeLookupFunction.hpp>
#include <inflection/grammar/PhraseDisplayFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void KnGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::grammar::GrammemeLookupFunction(::inflection::util::LocaleUtils::KANNADA(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}, {GrammemeConstants::POS_NOUN}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::grammar::GrammemeLookupFunction(::inflection::util::LocaleUtils::KANNADA(), {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}, GrammemeConstants::GENDER_NEUTER, true));
    featureModel.setDefaultDisplayFunction(new ::inflection::grammar::PhraseDisplayFunction(featureModel, ::inflection::util::LocaleUtils::KANNADA(), {
            {GrammemeConstants::POS_NOUN},
            {GrammemeConstants::CASE_NOMINATIVE, GrammemeConstants::CASE_GENITIVE, GrammemeConstants::CASE_VOCATIVE},
            {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
            {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}
    }, true));
}

} // namespace inflection::grammar::synthesis
