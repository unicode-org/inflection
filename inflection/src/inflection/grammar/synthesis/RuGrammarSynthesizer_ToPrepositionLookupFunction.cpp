/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_ToPrepositionLookupFunction.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_ToPrepositionLookupFunction::RuGrammarSynthesizer_ToPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

static constexpr const char16_t* TO_WORDS[] = {
    u"всему",
    u"всякому",
    u"всяческому",
    u"вторнику",
    u"второму",
    u"мне",
    u"многим",
    u"многому",
};

inflection::dialog::SpeakableString* RuGrammarSynthesizer_ToPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::inflection::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::inflection::util::LocaleUtils::ROOT());
    if (inflection::util::ArrayUtils::contains<TO_WORDS>(displayStringLowerCase)) {
        return createPreposition(displayValue, u"ко");
    }
    return createPreposition(displayValue, u"к");
}

} // namespace inflection::grammar::synthesis
