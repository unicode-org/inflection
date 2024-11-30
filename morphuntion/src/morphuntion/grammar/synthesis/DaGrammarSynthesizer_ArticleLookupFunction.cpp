/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

DaGrammarSynthesizer_ArticleLookupFunction::DaGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString)
    : DaGrammarSynthesizer_ArticleLookupFunction(model, includeSemanticValue, ::morphuntion::dialog::SpeakableString(singularCommonString), ::morphuntion::dialog::SpeakableString(singularNeuterString), ::morphuntion::dialog::SpeakableString(pluralString))
{
}

DaGrammarSynthesizer_ArticleLookupFunction::DaGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::morphuntion::dialog::SpeakableString& singularCommonString, const ::morphuntion::dialog::SpeakableString& singularNeuterString, const ::morphuntion::dialog::SpeakableString& pluralString)
    : super(model, includeSemanticValue, true)
    , singularCommonString(singularCommonString)
    , singularNeuterString(singularNeuterString)
    , pluralString(pluralString)
    , countLookupFunction(::morphuntion::util::LocaleUtils::DANISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
    , genderLookupFunction(::morphuntion::util::LocaleUtils::DANISH(), {GrammemeConstants::GENDER_COMMON(), GrammemeConstants::GENDER_NEUTER()})
{
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
}

morphuntion::dialog::SpeakableString* DaGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = DaGrammarSynthesizer::getCount(displayValue.getFeatureValue(*npc(countFeature)));
    if (DaGrammarSynthesizer::Count::undefined == countValue) {
        ::std::unique_ptr<morphuntion::dialog::SpeakableString> countSpeakableString(countLookupFunction.getFeatureValue(displayValue, constraints));
        countValue = countSpeakableString != nullptr ? DaGrammarSynthesizer::getCount(&countSpeakableString->getPrint()) : DaGrammarSynthesizer::Count::singular;
    }
    if (DaGrammarSynthesizer::Count::plural == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    auto genderValue = DaGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (DaGrammarSynthesizer::Gender::undefined == genderValue) {
        ::std::unique_ptr<morphuntion::dialog::SpeakableString> genderSpeakableString(genderLookupFunction.getFeatureValue(displayValue, constraints));
        genderValue = genderSpeakableString != nullptr ? DaGrammarSynthesizer::getGender(&genderSpeakableString->getPrint()) : DaGrammarSynthesizer::Gender::common;
    }
    if (DaGrammarSynthesizer::Gender::neuter == genderValue) {
        return createPreposition(displayValue, singularNeuterString);
    }
    return createPreposition(displayValue, singularCommonString);
}

} // namespace morphuntion::grammar::synthesis
