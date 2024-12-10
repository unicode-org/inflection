/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

EnGrammarSynthesizer_ArticleLookupFunction::EnGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName)
    : super(model, derivedSemanticName != nullptr, true)
    , derivedArticleFeature(derivedSemanticName != nullptr ? model.getFeature(npc(derivedSemanticName)) : static_cast<const ::morphuntion::dialog::SemanticFeature* >(nullptr))
{
}

morphuntion::dialog::SpeakableString* EnGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
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
        if (countValue != GrammemeConstants::NUMBER_PLURAL()) {
            if (displayString.length() > 3 && (::morphuntion::util::StringViewUtils::endsWith(displayString, u"’s") || ::morphuntion::util::StringViewUtils::endsWith(displayString, u"'s"))) {
                displayString.resize(displayString.length() - 2);
            }
            if (::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ENGLISH(), displayString)
                || displayString[0] == u'8')
            {
                return createPreposition(displayValue, u"an");
            } else {
                return createPreposition(displayValue, u"a");
            }
        }
        // The indefinite form doesn't really apply to plural forms.
    }
    return createPreposition(displayValue, {});
}

} // namespace morphuntion::grammar::synthesis
