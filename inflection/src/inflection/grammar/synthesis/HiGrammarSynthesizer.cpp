/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HiGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/HiGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/grammar/synthesis/HiGrammarSynthesizer_GenderLookupFunction.hpp>
#include <inflection/grammar/synthesis/HiGrammarSynthesizer_HiDisplayFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::grammar::synthesis {

void HiGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new HiGrammarSynthesizer_CountLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new HiGrammarSynthesizer_GenderLookupFunction());
    featureModel.setDefaultDisplayFunction(new HiGrammarSynthesizer_HiDisplayFunction(featureModel));
}

} // namespace inflection::grammar::synthesis
