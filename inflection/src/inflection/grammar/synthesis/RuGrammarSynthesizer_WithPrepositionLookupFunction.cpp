/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_WithPrepositionLookupFunction.hpp>

#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_WithPrepositionLookupFunction::RuGrammarSynthesizer_WithPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

static constexpr auto WITH_FIRST_CHAR = u"сжзш";
static constexpr const char16_t* WITH_WORDS[] = {
    u"всем",
    u"всеми",
    u"всех",
    u"всяким",
    u"всякими",
    u"всяких",
    u"всяческим",
    u"всяческими",
    u"вторник",
    u"вторника",
    u"второго",
    u"вторых",
    u"многих",
    u"мной",
    u"мною",
};

inflection::dialog::SpeakableString* RuGrammarSynthesizer_WithPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::inflection::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::inflection::util::LocaleUtils::ROOT());
    if (!displayString.empty()
        && (inflection::util::ArrayUtils::contains<WITH_WORDS>(displayStringLowerCase) || displayStringLowerCase[0] == u'щ' || RuGrammarSynthesizer::startsWith2Consonant(displayStringLowerCase, WITH_FIRST_CHAR) || (displayString.length() > 1 && displayStringLowerCase[0] == u'в' && displayStringLowerCase[1] == u'ч')))
    {
        return createPreposition(displayValue, u"со");
    }
    return createPreposition(displayValue, u"с");
}

} // namespace inflection::grammar::synthesis
