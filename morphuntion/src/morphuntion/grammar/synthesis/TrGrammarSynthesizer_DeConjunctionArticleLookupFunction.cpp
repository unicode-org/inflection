/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer_DeConjunctionArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer_TrDisplayFunction.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

TrGrammarSynthesizer_DeConjunctionArticleLookupFunction::TrGrammarSynthesizer_DeConjunctionArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const TrGrammarSynthesizer_TrDisplayFunction* displayFunctionUtility)
    : super(model, false, true)
    , displayFunction(displayFunctionUtility)
{
}

morphuntion::dialog::SpeakableString* TrGrammarSynthesizer_DeConjunctionArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string suffixString;
    auto vowGroup = npc(displayFunction)->getVowelGroup(displayString);
    if ((vowGroup == TrGrammarSynthesizer::VowelGroup::FRONT_UNROUNDED) || (vowGroup == TrGrammarSynthesizer::VowelGroup::FRONT_ROUNDED)) {
        suffixString += u" de";
    }
    else if ((vowGroup == TrGrammarSynthesizer::VowelGroup::BACK_UNROUNDED) || (vowGroup == TrGrammarSynthesizer::VowelGroup::BACK_ROUNDED)) {
        suffixString += u" da";
    }

    return new ::morphuntion::dialog::SpeakableString(::morphuntion::dialog::SpeakableString(displayString) + ::morphuntion::dialog::SpeakableString(suffixString));
}

} // namespace morphuntion::grammar::synthesis
