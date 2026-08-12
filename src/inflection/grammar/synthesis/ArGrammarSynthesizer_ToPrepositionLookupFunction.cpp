/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ToPrepositionLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <unicode/uscript.h>

namespace inflection::grammar::synthesis {

ArGrammarSynthesizer_ToPrepositionLookupFunction::ArGrammarSynthesizer_ToPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model)
{
}

static constexpr char16_t BA[] = u"ل";
static constexpr char16_t BA_WITH_KASHEDA[] = u"لـ\u00A0";

inflection::dialog::SpeakableString* ArGrammarSynthesizer_ToPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::Number /*countValue*/, ArGrammarSynthesizer::Gender /*genderValue*/, ArGrammarSynthesizer::Person /*personValue*/) const
{
    ::std::u16string article;
    std::u16string displayString(::inflection::util::StringViewUtils::trim(displayValue.getDisplayString()));
    if (!displayString.empty()) {
        if (uscript_hasScript(displayString[0], USCRIPT_ARABIC)) {
            if (displayString.length() > 3 && displayString[0] == u'\u0627' && displayString[1] == u'\u0644') {
                displayString = displayString.substr(1);
            }
            article = BA;
        } else {
            article = BA_WITH_KASHEDA;
        }
        return new ::inflection::dialog::SpeakableString(article + displayString);
    }
    return new ::inflection::dialog::SpeakableString(::std::u16string());
}

} // namespace inflection::grammar::synthesis
