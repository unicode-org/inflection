/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

ItGrammarSynthesizer_ArticleLookupFunction::ItGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, !derivedSemanticName.empty(), false)
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
{
    this->derivedArticleFeature = (!derivedSemanticName.empty() ? model.getFeature(derivedSemanticName) : static_cast<const ::morphuntion::dialog::SemanticFeature* >(nullptr));
}

morphuntion::dialog::SpeakableString* ItGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    auto countValue = ItGrammarSynthesizer::getCount(displayValue.getFeatureValue(countFeature));
    auto genderValue = ItGrammarSynthesizer::getGender(displayValue.getFeatureValue(genderFeature));
    const auto& displayString = displayValue.getDisplayString();
    if (countValue == ItGrammarSynthesizer::Count::undefined || genderValue == ItGrammarSynthesizer::Gender::undefined) {
        if (countValue == ItGrammarSynthesizer::Count::undefined) {
            auto value = countLookupFunction.determine(displayString);
            countValue = ItGrammarSynthesizer::getCount(&value);
        }
        if (genderValue == ItGrammarSynthesizer::Gender::undefined) {
            auto value = genderLookupFunction.determine(displayString);
            genderValue = ItGrammarSynthesizer::getGender(&value);
        }
    }
    return getArticle(displayValue, true, countValue, genderValue);
}

} // namespace morphuntion::grammar::synthesis
