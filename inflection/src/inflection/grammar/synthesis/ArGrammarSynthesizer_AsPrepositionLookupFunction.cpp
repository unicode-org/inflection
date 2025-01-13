/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_AsPrepositionLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <icu4cxx/UnicodeSet.hpp>

namespace inflection::grammar::synthesis {


ArGrammarSynthesizer_AsPrepositionLookupFunction::ArGrammarSynthesizer_AsPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model)
{
}

static constexpr char16_t KA_WITH_KASHEDA[] = u"كـ\u00A0";
static constexpr char16_t KA[] = u"ك";

inflection::dialog::SpeakableString* ArGrammarSynthesizer_AsPrepositionLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber /*countValue*/, ArGrammarSynthesizer::PronounGender /*genderValue*/, ArGrammarSynthesizer::Person /*personValue*/) const
{
    ::std::u16string article;
    std::u16string displayString(::inflection::util::StringViewUtils::trim(displayValue.getDisplayString()));
    if (!displayString.empty()) {
        if (::inflection::lang::StringFilterUtil::ARABIC_SCRIPT().contains(displayString[0])) {
            article = KA;
        } else {
            article = KA_WITH_KASHEDA;
        }
        return new ::inflection::dialog::SpeakableString(article + displayString);
    }
    return new ::inflection::dialog::SpeakableString(::std::u16string());
}

} // namespace inflection::grammar::synthesis
