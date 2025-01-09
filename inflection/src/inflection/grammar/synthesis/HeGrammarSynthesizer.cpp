/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HeGrammarSynthesizer.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_DefinitenessLookupFunction.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_GenderLookupFunction.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_HeDisplayFunction.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_WithConditionalHyphen.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

void HeGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
{
    featureModel.setDefaultDisplayFunction(new HeGrammarSynthesizer_HeDisplayFunction(featureModel));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::GENDER, new HeGrammarSynthesizer_GenderLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::NUMBER, new ::inflection::dialog::DictionaryLookupFunction(::inflection::util::LocaleUtils::HEBREW(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL(), GrammemeConstants::NUMBER_DUAL()}));
    featureModel.putDefaultFeatureFunctionByName(GrammemeConstants::DEFINITENESS, new HeGrammarSynthesizer_DefinitenessLookupFunction());
    featureModel.putDefaultFeatureFunctionByName(WITH_CONDITIONAL_HYPHEN, new HeGrammarSynthesizer_WithConditionalHyphen());
}

} // namespace inflection::grammar::synthesis
