/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/TeGrammarSynthesizer.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/GrammemeLookupFunction.hpp>
#include <inflection/grammar/PhraseDisplayFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void TeGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::grammar::GrammemeLookupFunction(::inflection::util::LocaleUtils::TELUGU(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}, {GrammemeConstants::POS_NOUN}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new ::inflection::grammar::GrammemeLookupFunction(::inflection::util::LocaleUtils::TELUGU(), {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}, GrammemeConstants::GENDER_NEUTER, true));
    featureModel.setDefaultDisplayFunction(new ::inflection::grammar::PhraseDisplayFunction(featureModel, ::inflection::util::LocaleUtils::TELUGU(), {
            {GrammemeConstants::POS_NOUN},
            {GrammemeConstants::CASE_DIRECT, GrammemeConstants::CASE_OBLIQUE},
            {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL},
            {GrammemeConstants::GENDER_MASCULINE, GrammemeConstants::GENDER_FEMININE, GrammemeConstants::GENDER_NEUTER}
    }, true));
}

} // namespace inflection::grammar::synthesis
