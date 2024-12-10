/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/HiGrammarSynthesizer.hpp>

#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/HiGrammarSynthesizer_CountLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/HiGrammarSynthesizer_GenderLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/HiGrammarSynthesizer_HiDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::grammar::synthesis {

void HiGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new HiGrammarSynthesizer_CountLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new HiGrammarSynthesizer_GenderLookupFunction());
    featureModel.setDefaultDisplayFunction(new HiGrammarSynthesizer_HiDisplayFunction(featureModel));
}

} // namespace morphuntion::grammar::synthesis
