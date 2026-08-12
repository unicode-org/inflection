/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FiGrammarSynthesizer_WithHyphenSuffixLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>

namespace inflection::grammar::synthesis {


FiGrammarSynthesizer_WithHyphenSuffixLookupFunction::FiGrammarSynthesizer_WithHyphenSuffixLookupFunction()
    : super()
{
}

inflection::dialog::SpeakableString* FiGrammarSynthesizer_WithHyphenSuffixLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (displayString.find(u' ') != ::std::u16string::npos) {
        return new ::inflection::dialog::SpeakableString(displayString + u" ");
    }
    return new ::inflection::dialog::SpeakableString(displayString);
}

} // namespace inflection::grammar::synthesis
