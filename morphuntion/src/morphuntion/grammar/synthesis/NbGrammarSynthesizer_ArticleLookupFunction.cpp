/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NbGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/NbGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

NbGrammarSynthesizer_ArticleLookupFunction::NbGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString)
    : super(model, includeSemanticValue, true)
    , singularMasculineString(singularMasculineString)
    , singularFeminineString(singularFeminineString)
    , singularNeuterString(singularNeuterString)
    , pluralString(pluralString)
    , countLookupFunction(::morphuntion::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
    , genderLookupFunction(::morphuntion::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()})
{
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
}

morphuntion::dialog::SpeakableString* NbGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = NbGrammarSynthesizer::getCount(displayValue.getFeatureValue(*npc(countFeature)));
    if (NbGrammarSynthesizer::Count::UNDEFINED == countValue) {
        ::std::unique_ptr<morphuntion::dialog::SpeakableString> countSpeakableString(countLookupFunction.getFeatureValue(displayValue, constraints));
        countValue = countSpeakableString != nullptr ? NbGrammarSynthesizer::getCount(&countSpeakableString->getPrint()) : NbGrammarSynthesizer::Count::SINGULAR;
    }
    if (NbGrammarSynthesizer::Count::PLURAL == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    auto genderValue = NbGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (NbGrammarSynthesizer::Gender::UNDEFINED == genderValue) {
        ::std::unique_ptr<morphuntion::dialog::SpeakableString> genderSpeakableString(genderLookupFunction.getFeatureValue(displayValue, constraints));
        genderValue = genderSpeakableString != nullptr ? NbGrammarSynthesizer::getGender(&genderSpeakableString->getPrint()) : NbGrammarSynthesizer::Gender::MASCULINE;
    }
    if (NbGrammarSynthesizer::Gender::NEUTER == genderValue) {
        return createPreposition(displayValue, singularNeuterString);
    }
    if (NbGrammarSynthesizer::Gender::FEMININE == genderValue) {
        return createPreposition(displayValue, singularFeminineString);
    }
    return createPreposition(displayValue, singularMasculineString);
}

} // namespace morphuntion::grammar::synthesis
