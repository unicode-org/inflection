/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_PronounLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <unicode/uscript.h>

namespace inflection::grammar::synthesis {

ArGrammarSynthesizer_PronounLookupFunction::ArGrammarSynthesizer_PronounLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model)
{
}

static constexpr char16_t TEH_MARBUTA { u'\u0629' };
static constexpr char16_t TEH[] = u"ت";

inflection::dialog::SpeakableString* ArGrammarSynthesizer_PronounLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::Number countValue, ArGrammarSynthesizer::Gender genderValue, ArGrammarSynthesizer::Person personValue) const
{
    auto value = ArGrammarSynthesizer::getPossessivePronoun(ArGrammarSynthesizer::makeLookupKey(countValue, genderValue, personValue));
    auto displayString = displayValue.getDisplayString();
    int32_t lastIndexDisplayString = int32_t(displayString.length() - 1);
    if (lastIndexDisplayString > 1 && displayString[lastIndexDisplayString] == TEH_MARBUTA) {
        displayString = displayString.substr(0, lastIndexDisplayString) + TEH;
    }
    if (value != nullptr && !displayString.empty() && ::inflection::util::StringViewUtils::trim(displayString).find(u' ') == ::std::u16string::npos && !static_cast<bool>(uscript_hasScript(displayString[0], USCRIPT_LATIN)) && !displayString.starts_with(u"ال") && (!displayString.ends_with(u"ي") || u"أخي" == displayString || u"أبي" == displayString)) {
        displayString += value;
    }
    return new ::inflection::dialog::SpeakableString(displayString);
}

} // namespace inflection::grammar::synthesis
