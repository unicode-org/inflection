/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

static constexpr auto STRESSED = u"stressed";

EsGrammarSynthesizer_ArticleLookupFunction::EsGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, ::std::u16string_view defaultString, ::std::u16string_view singularMasculineString, ::std::u16string_view singularFeminineString, ::std::u16string_view pluralMasculineString, ::std::u16string_view pluralFeminineString, bool hasStressedSingularFeminineForm)
    : super(model, derivedSemanticName != nullptr, true)
    , countLookupFunction(EsGrammarSynthesizer_CountGenderLookupFunction::GrammarCategoryType::NUMBER_CATEGORY, {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
    , genderLookupFunction(EsGrammarSynthesizer_CountGenderLookupFunction::GrammarCategoryType::GENDER_CATEGORY, {GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_MASCULINE()})
    , stressLookupFunction(::inflection::util::LocaleUtils::SPANISH(), {STRESSED})
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , defaultString(defaultString)
    , singularMasculineString(singularMasculineString)
    , singularFeminineString(singularFeminineString)
    , pluralMasculineString(pluralMasculineString)
    , pluralFeminineString(pluralFeminineString)
    , hasStressedSingularFeminineForm(hasStressedSingularFeminineForm)
{
    this->derivedArticleFeature = (derivedSemanticName != nullptr ? model.getFeature(npc(derivedSemanticName)) : static_cast< ::inflection::dialog::SemanticFeature* >(nullptr));
}

EsGrammarSynthesizer_ArticleLookupFunction::EsGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, ::std::u16string_view defaultString, ::std::u16string_view singularMasculineString, ::std::u16string_view singularFeminineString, ::std::u16string_view pluralMasculineString, ::std::u16string_view pluralFeminineString)
    : EsGrammarSynthesizer_ArticleLookupFunction(model, derivedSemanticName, defaultString, singularMasculineString, singularFeminineString, pluralMasculineString, pluralFeminineString, false)
{
}

EsGrammarSynthesizer_ArticleLookupFunction::EsGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, const EsGrammarSynthesizer_ArticleLookupFunction& other)
        : EsGrammarSynthesizer_ArticleLookupFunction(model, derivedSemanticName, other.defaultString, other.singularMasculineString, other.singularFeminineString, other.pluralMasculineString, other.pluralFeminineString, other.hasStressedSingularFeminineForm)
{
}

inflection::dialog::SpeakableString* EsGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    const auto& displayString = displayValue.getDisplayString();
    auto countValue = EsGrammarSynthesizer::getNumber(displayValue.getFeatureValue(countFeature));
    auto genderValue = EsGrammarSynthesizer::getGender(displayValue.getFeatureValue(genderFeature));
    if (countValue == EsGrammarSynthesizer::Number::undefined) {
        auto countStr(countLookupFunction.determine(displayString));
        countValue = EsGrammarSynthesizer::getNumber(&countStr);
    }
    if (genderValue == EsGrammarSynthesizer::Gender::undefined) {
        auto genderStr(genderLookupFunction.determine(displayString));
        genderValue = EsGrammarSynthesizer::getGender(&genderStr);
    }
    if (EsGrammarSynthesizer::Number::singular == countValue) {
        if (EsGrammarSynthesizer::Gender::feminine == genderValue
            && (!hasStressedSingularFeminineForm || stressLookupFunction.determine(stressLookupFunction.getFirstWord(displayString)) != STRESSED))
        {
            return createPreposition(displayValue, ::std::u16string(singularFeminineString));
        }
        // else use the masculine form.
        if (EsGrammarSynthesizer::Gender::masculine == genderValue || EsGrammarSynthesizer::Gender::feminine == genderValue) {
            return createPreposition(displayValue, ::std::u16string(singularMasculineString));
        }
    }
    if (EsGrammarSynthesizer::Number::plural == countValue && EsGrammarSynthesizer::Gender::masculine == genderValue) {
        return createPreposition(displayValue, ::std::u16string(pluralMasculineString));
    }
    if (EsGrammarSynthesizer::Number::plural == countValue && EsGrammarSynthesizer::Gender::feminine == genderValue) {
        return createPreposition(displayValue, ::std::u16string(pluralFeminineString));
    }
    return createPreposition(displayValue, ::std::u16string(defaultString));
}

const char16_t *EsGrammarSynthesizer_ArticleLookupFunction::getDerivedSemanticName() const {
    return derivedArticleFeature->getName().c_str();
}

} // namespace inflection::grammar::synthesis
