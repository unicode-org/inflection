/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_WithPrepositionLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

RuGrammarSynthesizer_WithPrepositionLookupFunction::RuGrammarSynthesizer_WithPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

const ::icu4cxx::UnicodeSet& RuGrammarSynthesizer_WithPrepositionLookupFunction::WITH_FIRST_CHAR()
{
    static auto WITH_FIRST_CHAR_= ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[сжзш]", USET_CASE_INSENSITIVE));
    return *npc(WITH_FIRST_CHAR_);
}

const ::std::set<::std::u16string_view>& RuGrammarSynthesizer_WithPrepositionLookupFunction::WITH_WORDS()
{
    static auto WITH_WORDS_ = new ::std::set<::std::u16string_view>({
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
    });
    return *npc(WITH_WORDS_);
}

morphuntion::dialog::SpeakableString* RuGrammarSynthesizer_WithPrepositionLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::morphuntion::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::morphuntion::util::LocaleUtils::ROOT());
    if (!displayString.empty()
        && (WITH_WORDS().count(displayStringLowerCase) != 0 || displayStringLowerCase[0] == u'\u0449' || RuGrammarSynthesizer::startsWith2Consonant(displayStringLowerCase, WITH_FIRST_CHAR()) || (displayString.length() > 1 && displayStringLowerCase[0] == u'\u0432' && displayStringLowerCase[1] == u'\u0447')))
    {
        return createPreposition(displayValue, u"со");
    }
    return createPreposition(displayValue, u"с");
}

} // namespace morphuntion::grammar::synthesis
