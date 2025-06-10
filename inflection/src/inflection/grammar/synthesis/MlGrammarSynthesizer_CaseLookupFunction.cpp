/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_CaseLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

MlGrammarSynthesizer_CaseLookupFunction::MlGrammarSynthesizer_CaseLookupFunction()
    : super()
{
    // No file needed
}

inflection::dialog::SpeakableString* MlGrammarSynthesizer_CaseLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    std::u16string displayString;
    ::inflection::util::StringViewUtils::lowercase(&displayString, displayValue.getDisplayString(), ::inflection::util::LocaleUtils::MALAYALAM());

    if (displayString.length() >= 3) {
        // Genitive-indicative suffixes in Malayalam
        if (displayString.ends_with(u"ഉടെ") ||   // uṭe
            displayString.ends_with(u"യുടെ") || // yude (my, your, his, her...)
            displayString.ends_with(u"ന്റെ") || // ente (mine), avante, etc.
            displayString.ends_with(u"ആയുടെ"))  // āyuṭe (fem. 3rd person possessive)
        {
            return new ::inflection::dialog::SpeakableString(GrammemeConstants::CASE_GENITIVE());
        }
    }
    return nullptr;
}

} // namespace inflection::grammar::synthesis

