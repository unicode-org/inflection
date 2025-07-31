/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

ItGrammarSynthesizer_ArticleLookupFunction::ItGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, !derivedSemanticName.empty(), false)
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
{
    this->derivedArticleFeature = (!derivedSemanticName.empty() ? model.getFeature(derivedSemanticName) : static_cast<const ::inflection::dialog::SemanticFeature* >(nullptr));
}

inflection::dialog::SpeakableString* ItGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    auto countValue = ItGrammarSynthesizer::getNumber(displayValue.getFeatureValue(countFeature));
    auto genderValue = ItGrammarSynthesizer::getGender(displayValue.getFeatureValue(genderFeature));
    const auto& displayString = displayValue.getDisplayString();
    if (countValue == ItGrammarSynthesizer::Number::undefined || genderValue == ItGrammarSynthesizer::Gender::undefined) {
        if (countValue == ItGrammarSynthesizer::Number::undefined) {
            auto value = countLookupFunction.determine(displayString);
            countValue = ItGrammarSynthesizer::getNumber(&value);
        }
        if (genderValue == ItGrammarSynthesizer::Gender::undefined) {
            auto value = genderLookupFunction.determine(displayString);
            genderValue = ItGrammarSynthesizer::getGender(&value);
        }
    }
    return getArticle(displayValue, true, countValue, genderValue);
}

} // namespace inflection::grammar::synthesis
