/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_ToPrepositionLookupFunction.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_ToPrepositionLookupFunction::RuGrammarSynthesizer_ToPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
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

inflection::dialog::SpeakableString* RuGrammarSynthesizer_ToPrepositionLookupFunction::getArticle(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::inflection::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::inflection::util::LocaleUtils::ROOT());
    if (TO_WORDS().count(displayStringLowerCase) != 0) {
        return createPreposition(displayValue, u"ко");
    }
    return createPreposition(displayValue, u"к");
}

} // namespace inflection::grammar::synthesis
