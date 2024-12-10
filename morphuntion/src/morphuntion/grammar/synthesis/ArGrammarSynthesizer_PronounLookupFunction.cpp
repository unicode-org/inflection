/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer_PronounLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <icu4cxx/UnicodeSet.hpp>

namespace morphuntion::grammar::synthesis {

ArGrammarSynthesizer_PronounLookupFunction::ArGrammarSynthesizer_PronounLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::map<int32_t, ::std::u16string_view>& articleMap)
    : super(model)
    , articleMap(articleMap)
{
}

static constexpr char16_t TEH_MARBUTA { u'\u0629' };
static constexpr char16_t TEH[] = u"ت";

morphuntion::dialog::SpeakableString* ArGrammarSynthesizer_PronounLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber countValue, ArGrammarSynthesizer::PronounGender genderValue, ArGrammarSynthesizer::Person personValue) const
{
    auto value = articleMap.find(ArGrammarSynthesizer::makeLookupKey(countValue, genderValue, personValue).value);
    auto displayString = displayValue.getDisplayString();
    int32_t lastIndexDisplayString = int32_t(displayString.length() - 1);
    if (lastIndexDisplayString > 1 && displayString[lastIndexDisplayString] == TEH_MARBUTA) {
        displayString = displayString.substr(0, lastIndexDisplayString) + TEH;
    }
    if (value != articleMap.end() && !displayString.empty() && ::morphuntion::util::StringViewUtils::trim(displayString).find(u' ') == ::std::u16string::npos && !::morphuntion::lang::StringFilterUtil::LATIN_SCRIPT().contains(displayString[0]) && !::morphuntion::util::StringViewUtils::startsWith(displayString, u"ال") && (!::morphuntion::util::StringViewUtils::endsWith(displayString, u"ي") || u"أخي" == displayString || u"أبي" == displayString)) {
        displayString += value->second;
    }
    return new ::morphuntion::dialog::SpeakableString(displayString);
}

} // namespace morphuntion::grammar::synthesis
