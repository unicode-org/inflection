/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/grammar/synthesis/BnGrammarSynthesizer.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/GrammemeLookupFunction.hpp>
#include <inflection/grammar/synthesis/BnGrammarSynthesizer_BnDisplayFunction.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void BnGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::grammar::GrammemeLookupFunction(::inflection::util::LocaleUtils::BANGLA(), {GrammemeConstants::NUMBER_SINGULAR, GrammemeConstants::NUMBER_PLURAL}, {GrammemeConstants::POS_NOUN}));
    featureModel.setDefaultDisplayFunction(new BnGrammarSynthesizer_BnDisplayFunction(featureModel));
}

} // namespace inflection::grammar::synthesis
