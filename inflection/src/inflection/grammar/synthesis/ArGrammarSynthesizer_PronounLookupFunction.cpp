/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_PronounLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>

namespace inflection::grammar::synthesis {

ArGrammarSynthesizer_PronounLookupFunction::ArGrammarSynthesizer_PronounLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& articleMap)
    : super(model)
    , articleMap(articleMap)
{
}

static constexpr char16_t TEH_MARBUTA { u'\u0629' };
static constexpr char16_t TEH[] = u"ت";

inflection::dialog::SpeakableString* ArGrammarSynthesizer_PronounLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber countValue, ArGrammarSynthesizer::PronounGender genderValue, ArGrammarSynthesizer::Person personValue) const
{
    auto value = articleMap.find(ArGrammarSynthesizer::makeLookupKey(countValue, genderValue, personValue).value);
    auto displayString = displayValue.getDisplayString();
    int32_t lastIndexDisplayString = int32_t(displayString.length() - 1);
    if (lastIndexDisplayString > 1 && displayString[lastIndexDisplayString] == TEH_MARBUTA) {
        displayString = displayString.substr(0, lastIndexDisplayString) + TEH;
    }
    if (value != articleMap.end() && !displayString.empty() && ::inflection::util::StringViewUtils::trim(displayString).find(u' ') == ::std::u16string::npos && !::inflection::lang::StringFilterUtil::LATIN_SCRIPT().contains(displayString[0]) && !displayString.starts_with(u"ال") && (!displayString.ends_with(u"ي") || u"أخي" == displayString || u"أبي" == displayString)) {
        displayString += value->second;
    }
    return new ::inflection::dialog::SpeakableString(displayString);
}

} // namespace inflection::grammar::synthesis
