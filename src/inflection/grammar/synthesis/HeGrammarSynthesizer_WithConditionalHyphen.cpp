/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_WithConditionalHyphen.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <unicode/uchar.h>
#include <unicode/uscript.h>

namespace inflection::grammar::synthesis {

HeGrammarSynthesizer_WithConditionalHyphen::HeGrammarSynthesizer_WithConditionalHyphen()
    : super()
{
}

inflection::dialog::SpeakableString* HeGrammarSynthesizer_WithConditionalHyphen::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();

    UChar32 cp;
    const int32_t len = static_cast<int32_t>(displayString.length());
    for (int32_t idx = 0; idx < len; idx += U16_LENGTH(cp)) {
        cp = static_cast<UChar32>(::inflection::util::StringViewUtils::codePointAt(displayString, idx));
        if ((U_GET_GC_MASK(cp) & (U_GC_L_MASK | U_GC_ND_MASK)) != 0) {
            // It's a letter, or a decimal number
            if (static_cast<bool>(uscript_hasScript(cp, USCRIPT_HEBREW))) {
                return new ::inflection::dialog::SpeakableString(displayString);
            }
            break;
        }
    }

    return new ::inflection::dialog::SpeakableString(::inflection::dialog::SpeakableString(u"-") + ::inflection::dialog::SpeakableString(displayString));
}

} // namespace inflection::grammar::synthesis
