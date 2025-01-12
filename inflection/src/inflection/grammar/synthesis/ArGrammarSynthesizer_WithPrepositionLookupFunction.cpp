/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_WithPrepositionLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <icu4cxx/UnicodeSet.hpp>

namespace inflection::grammar::synthesis {

ArGrammarSynthesizer_WithPrepositionLookupFunction::ArGrammarSynthesizer_WithPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model)
{
}

static constexpr char16_t BA[] = u"ب";
static constexpr char16_t BA_WITH_KASHEDA[] = u"بـ\u00A0";

inflection::dialog::SpeakableString* ArGrammarSynthesizer_WithPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber /*countValue*/, ArGrammarSynthesizer::PronounGender /*genderValue*/, ArGrammarSynthesizer::Person /*personValue*/) const
{
    ::std::u16string article;
    std::u16string displayString(::inflection::util::StringViewUtils::trim(displayValue.getDisplayString()));
    if (!displayString.empty()) {
        if (::inflection::lang::StringFilterUtil::ARABIC_SCRIPT().contains(displayString[0])) {
            article = BA;
        }
        else {
            article = BA_WITH_KASHEDA;
        }
        return new ::inflection::dialog::SpeakableString(article + displayString);
    }
    return new ::inflection::dialog::SpeakableString(::std::u16string());
}

} // namespace inflection::grammar::synthesis
