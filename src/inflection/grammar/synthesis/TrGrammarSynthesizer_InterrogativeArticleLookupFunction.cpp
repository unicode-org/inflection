/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/TrGrammarSynthesizer_InterrogativeArticleLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer_TrDisplayFunction.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

TrGrammarSynthesizer_InterrogativeArticleLookupFunction::TrGrammarSynthesizer_InterrogativeArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const TrGrammarSynthesizer_TrDisplayFunction* displayFunctionUtility)
    : super(model, false, true)
    , displayFunction(displayFunctionUtility)
{
}

inflection::dialog::SpeakableString* TrGrammarSynthesizer_InterrogativeArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    auto vowGroup = npc(displayFunction)->getVowelGroup(displayString);
    return new ::inflection::dialog::SpeakableString(displayString + u" m" + TrGrammarSynthesizer::getAffix(vowGroup));
}


} // namespace inflection::grammar::synthesis
