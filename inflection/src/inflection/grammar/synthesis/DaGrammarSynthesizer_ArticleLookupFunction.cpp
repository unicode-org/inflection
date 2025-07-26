/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/DaGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/DaGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

DaGrammarSynthesizer_ArticleLookupFunction::DaGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString)
    : DaGrammarSynthesizer_ArticleLookupFunction(model, includeSemanticValue, ::inflection::dialog::SpeakableString(singularCommonString), ::inflection::dialog::SpeakableString(singularNeuterString), ::inflection::dialog::SpeakableString(pluralString))
{
}

DaGrammarSynthesizer_ArticleLookupFunction::DaGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::inflection::dialog::SpeakableString& singularCommonString, const ::inflection::dialog::SpeakableString& singularNeuterString, const ::inflection::dialog::SpeakableString& pluralString)
    : super(model, includeSemanticValue, true)
    , singularCommonString(singularCommonString)
    , singularNeuterString(singularNeuterString)
    , pluralString(pluralString)
    , countLookupFunction(::inflection::util::LocaleUtils::DANISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
    , genderLookupFunction(::inflection::util::LocaleUtils::DANISH(), {GrammemeConstants::GENDER_COMMON(), GrammemeConstants::GENDER_NEUTER()})
{
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
}

inflection::dialog::SpeakableString* DaGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = DaGrammarSynthesizer::getNumber(displayValue.getFeatureValue(*npc(countFeature)));
    if (DaGrammarSynthesizer::Number::undefined == countValue) {
        ::std::unique_ptr<inflection::dialog::SpeakableString> countSpeakableString(countLookupFunction.getFeatureValue(displayValue, constraints));
        countValue = countSpeakableString != nullptr ? DaGrammarSynthesizer::getNumber(&countSpeakableString->getPrint()) : DaGrammarSynthesizer::Number::singular;
    }
    if (DaGrammarSynthesizer::Number::plural == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    auto genderValue = DaGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (DaGrammarSynthesizer::Gender::undefined == genderValue) {
        ::std::unique_ptr<inflection::dialog::SpeakableString> genderSpeakableString(genderLookupFunction.getFeatureValue(displayValue, constraints));
        genderValue = genderSpeakableString != nullptr ? DaGrammarSynthesizer::getGender(&genderSpeakableString->getPrint()) : DaGrammarSynthesizer::Gender::common;
    }
    if (DaGrammarSynthesizer::Gender::neuter == genderValue) {
        return createPreposition(displayValue, singularNeuterString);
    }
    return createPreposition(displayValue, singularCommonString);
}

} // namespace inflection::grammar::synthesis
