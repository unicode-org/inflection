/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer_InterrogativeArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer_TrDisplayFunction.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

TrGrammarSynthesizer_InterrogativeArticleLookupFunction::TrGrammarSynthesizer_InterrogativeArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const TrGrammarSynthesizer_TrDisplayFunction* displayFunctionUtility)
    : super(model, false, true)
    , displayFunction(displayFunctionUtility)
{
}

morphuntion::dialog::SpeakableString* TrGrammarSynthesizer_InterrogativeArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    auto vowGroup = npc(displayFunction)->getVowelGroup(displayString);
    return new ::morphuntion::dialog::SpeakableString(displayString + u" m" + TrGrammarSynthesizer::getAffix(vowGroup));
}


} // namespace morphuntion::grammar::synthesis
