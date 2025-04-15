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

namespace inflection::grammar::synthesis {

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
        if (countValue != GrammemeConstants::NUMBER_PLURAL()) {
            if (displayString.length() > 3 && (displayString.ends_with(u"’s") || displayString.ends_with(u"'s"))) {
                displayString.resize(displayString.length() - 2);
            }
            if (::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ENGLISH(), displayString)
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

} // namespace inflection::grammar::synthesis
