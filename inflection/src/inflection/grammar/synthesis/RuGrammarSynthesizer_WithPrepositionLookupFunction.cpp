/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_WithPrepositionLookupFunction.hpp>

#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_WithPrepositionLookupFunction::RuGrammarSynthesizer_WithPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
    , withWords({
        u"мной",
        u"мною",
        u"всем",
        u"всех",
        u"всеми",
        u"всяким",
        u"всяких",
        u"всякими",
        u"всяческим",
        u"всяческими",
        u"вторника",
        u"вторник",
        u"второго",
        u"вторых",
        u"многих"
    })
{
}

const ::icu4cxx::UnicodeSet& RuGrammarSynthesizer_WithPrepositionLookupFunction::WITH_FIRST_CHAR()
{
    static auto WITH_FIRST_CHAR_= ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[сжзш]", USET_CASE_INSENSITIVE));
    return *npc(WITH_FIRST_CHAR_);
}

inflection::dialog::SpeakableString* RuGrammarSynthesizer_WithPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::inflection::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::inflection::util::LocaleUtils::ROOT());
    if (!displayString.empty()
        && (withWords.contains(displayStringLowerCase) || displayStringLowerCase[0] == u'\u0449' || RuGrammarSynthesizer::startsWith2Consonant(displayStringLowerCase, WITH_FIRST_CHAR()) || (displayString.length() > 1 && displayStringLowerCase[0] == u'\u0432' && displayStringLowerCase[1] == u'\u0447')))
    {
        return createPreposition(displayValue, u"со");
    }
    return createPreposition(displayValue, u"с");
}

} // namespace inflection::grammar::synthesis
