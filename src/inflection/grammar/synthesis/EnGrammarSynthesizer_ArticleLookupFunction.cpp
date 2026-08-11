/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/NumberFormat.hpp>
#include <unicode/uchar.h>

namespace inflection::grammar::synthesis {

static bool isStartsWithVowelDigits(::std::u16string_view str)
{
    if (u_isdigit(str.front())) {
        int64_t num = u_charDigitValue(str.front());
        if (num == 8) {
            // eight or eighty
            return true;
        }
        if (num == 1) {
            ::icu4cxx::NumberFormat numberFormat(UNUM_DECIMAL, ::std::u16string_view(), "en", nullptr);
            int32_t parsePos = 0;
            UErrorCode ec = U_ZERO_ERROR;
            num = unum_parseInt64(numberFormat.wrappee_, reinterpret_cast<const UChar *>(str.data()), static_cast<int32_t>(str.size()), &parsePos, &ec);
            if (static_cast<bool>(U_SUCCESS(ec)) && parsePos > 0 && num >= 0) {
                while (num >= 1000) {
                    num /= 1000;
                }
                if (num >= 100) {
                    // 121 --> 1, 800 --> 8
                    num /= 100;
                }
                // eleven, eighteen
                // not one (pronounced won)
                return num == 11 || num == 18;
            }
        }
    }
    return false;
}

EnGrammarSynthesizer_ArticleLookupFunction::EnGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName)
    : super(model, derivedSemanticName != nullptr, true)
    , derivedArticleFeature(derivedSemanticName != nullptr ? model.getFeature(npc(derivedSemanticName)) : static_cast<const ::inflection::dialog::SemanticFeature* >(nullptr))
{
}

inflection::dialog::SpeakableString* EnGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    auto displayString = displayValue.getDisplayString();
    if (!displayString.empty()) {
        auto countValue(countLookupFunction.determine(displayString));
        if (countValue != GrammemeConstants::NUMBER_PLURAL) {
            if (displayString.length() > 3 && (displayString.ends_with(u"’s") || displayString.ends_with(u"'s"))) {
                displayString.resize(displayString.length() - 2);
            }
            if (::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), displayString)
                || isStartsWithVowelDigits(displayString))
            {
                return createPreposition(displayValue, u"an");
            } else {
                return createPreposition(displayValue, u"a");
            }
        }
        // The indefinite form doesn't really apply to plural forms.
    }
    return createPreposition(displayValue, std::u16string());
}

} // namespace inflection::grammar::synthesis
