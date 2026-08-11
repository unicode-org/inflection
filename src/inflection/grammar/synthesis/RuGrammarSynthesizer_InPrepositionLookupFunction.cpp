/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_InPrepositionLookupFunction.hpp>

#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/util/ArrayUtils.hpp>

namespace inflection::grammar::synthesis {

RuGrammarSynthesizer_InPrepositionLookupFunction::RuGrammarSynthesizer_InPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

static constexpr auto IN_FIRST_CHAR = u"вфр";
static constexpr const char16_t* IN_WORDS[] = {
    u"мне",
    u"что",
};

inflection::dialog::SpeakableString* RuGrammarSynthesizer_InPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (inflection::util::ArrayUtils::contains<IN_WORDS>(displayString) || displayString.starts_with(u"мног") || displayString.starts_with(u"множ") || RuGrammarSynthesizer::startsWith2Consonant(displayString, IN_FIRST_CHAR)) {
        return createPreposition(displayValue, u"во");
    }
    return createPreposition(displayValue, u"в");
}

} // namespace inflection::grammar::synthesis
