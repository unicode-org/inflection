/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_InPrepositionLookupFunction.hpp>

#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_InPrepositionLookupFunction::RuGrammarSynthesizer_InPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

const ::std::set<::std::u16string_view>& RuGrammarSynthesizer_InPrepositionLookupFunction::IN_WORDS()
{
    static auto IN_WORDS_ = new ::std::set<::std::u16string_view>({
        u"что",
        u"мне"
    });
    return *npc(IN_WORDS_);
}

const ::icu4cxx::UnicodeSet& RuGrammarSynthesizer_InPrepositionLookupFunction::IN_FIRST_CHAR()
{
    static auto IN_FIRST_CHAR_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[вфр]", USET_CASE_INSENSITIVE));
    return *npc(IN_FIRST_CHAR_);
}

inflection::dialog::SpeakableString* RuGrammarSynthesizer_InPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (IN_WORDS().count(displayString) != 0 || displayString.starts_with(u"мног") || displayString.starts_with(u"множ") || RuGrammarSynthesizer::startsWith2Consonant(displayString, IN_FIRST_CHAR())) {
        return createPreposition(displayValue, u"во");
    }
    return createPreposition(displayValue, u"в");
}

} // namespace inflection::grammar::synthesis
