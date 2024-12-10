/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer_WithPrepositionLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <icu4cxx/UnicodeSet.hpp>

namespace morphuntion::grammar::synthesis {

ArGrammarSynthesizer_WithPrepositionLookupFunction::ArGrammarSynthesizer_WithPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super(model)
{
}

static constexpr char16_t BA[] = u"ب";
static constexpr char16_t BA_WITH_KASHEDA[] = u"بـ\u00A0";

morphuntion::dialog::SpeakableString* ArGrammarSynthesizer_WithPrepositionLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, ArGrammarSynthesizer::PronounNumber /*countValue*/, ArGrammarSynthesizer::PronounGender /*genderValue*/, ArGrammarSynthesizer::Person /*personValue*/) const
{
    ::std::u16string article;
    std::u16string displayString(::morphuntion::util::StringViewUtils::trim(displayValue.getDisplayString()));
    if (!displayString.empty()) {
        if (::morphuntion::lang::StringFilterUtil::ARABIC_SCRIPT().contains(displayString[0])) {
            article = BA;
        }
        else {
            article = BA_WITH_KASHEDA;
        }
        return new ::morphuntion::dialog::SpeakableString(article + displayString);
    }
    return new ::morphuntion::dialog::SpeakableString(::std::u16string());
}

} // namespace morphuntion::grammar::synthesis
