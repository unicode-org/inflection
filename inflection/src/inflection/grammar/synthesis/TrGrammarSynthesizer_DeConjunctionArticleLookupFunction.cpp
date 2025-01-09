/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/TrGrammarSynthesizer_DeConjunctionArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer_TrDisplayFunction.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

TrGrammarSynthesizer_DeConjunctionArticleLookupFunction::TrGrammarSynthesizer_DeConjunctionArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const TrGrammarSynthesizer_TrDisplayFunction* displayFunctionUtility)
    : super(model, false, true)
    , displayFunction(displayFunctionUtility)
{
}

inflection::dialog::SpeakableString* TrGrammarSynthesizer_DeConjunctionArticleLookupFunction::getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
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

    return new ::inflection::dialog::SpeakableString(::inflection::dialog::SpeakableString(displayString) + ::inflection::dialog::SpeakableString(suffixString));
}

} // namespace inflection::grammar::synthesis
