/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_InPrepositionLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

RuGrammarSynthesizer_InPrepositionLookupFunction::RuGrammarSynthesizer_InPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
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
    static auto IN_FIRST_CHAR_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[вфр]", USET_CASE_INSENSITIVE));
    return *npc(IN_FIRST_CHAR_);
}

morphuntion::dialog::SpeakableString* RuGrammarSynthesizer_InPrepositionLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (IN_WORDS().count(displayString) != 0 || ::morphuntion::util::StringViewUtils::startsWith(displayString, u"мног") || ::morphuntion::util::StringViewUtils::startsWith(displayString, u"множ") || RuGrammarSynthesizer::startsWith2Consonant(displayString, IN_FIRST_CHAR())) {
        return createPreposition(displayValue, u"во");
    }
    return createPreposition(displayValue, u"в");
}

} // namespace morphuntion::grammar::synthesis
