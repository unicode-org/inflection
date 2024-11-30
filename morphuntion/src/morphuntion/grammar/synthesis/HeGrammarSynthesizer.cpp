/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer.hpp>

#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer_DefinitenessLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer_GenderLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer_HeDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer_WithConditionalHyphen.hpp>
#include <morphuntion/util/LocaleUtils.hpp>

namespace morphuntion::grammar::synthesis {

void HeGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.setDefaultDisplayFunction(new HeGrammarSynthesizer_HeDisplayFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new HeGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::morphuntion::dialog::DictionaryLookupFunction(::morphuntion::util::LocaleUtils::HEBREW(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL(), GrammemeConstants::NUMBER_DUAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new HeGrammarSynthesizer_DefinitenessLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(WITH_CONDITIONAL_HYPHEN, new HeGrammarSynthesizer_WithConditionalHyphen());
}

} // namespace morphuntion::grammar::synthesis
