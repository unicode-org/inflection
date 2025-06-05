/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_GenderLookupFunction.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_CaseLookupFunction.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_MlDisplayFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::grammar::synthesis {

void MlGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new MlGrammarSynthesizer_CountLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new MlGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::CASE, new MlGrammarSynthesizer_CaseLookupFunction());
    
    featureModel.setDefaultDisplayFunction(new MlGrammarSynthesizer_MlDisplayFunction(featureModel));
}

} // namespace inflection::grammar::synthesis

