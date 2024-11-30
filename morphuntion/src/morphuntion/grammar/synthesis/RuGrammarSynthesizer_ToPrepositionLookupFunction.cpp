/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_ToPrepositionLookupFunction.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

RuGrammarSynthesizer_ToPrepositionLookupFunction::RuGrammarSynthesizer_ToPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

const ::std::set<::std::u16string_view>& RuGrammarSynthesizer_ToPrepositionLookupFunction::TO_WORDS()
{
    static auto TO_WORDS_ = new ::std::set<::std::u16string_view>({
            u"мне",
            u"всему",
            u"всякому",
            u"всяческому",
            u"вторнику",
            u"второму",
            u"многому",
            u"многим"
    });
    return *npc(TO_WORDS_);
}

morphuntion::dialog::SpeakableString* RuGrammarSynthesizer_ToPrepositionLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::morphuntion::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::morphuntion::util::LocaleUtils::ROOT());
    if (TO_WORDS().count(displayStringLowerCase) != 0) {
        return createPreposition(displayValue, u"ко");
    }
    return createPreposition(displayValue, u"к");
}

} // namespace morphuntion::grammar::synthesis
