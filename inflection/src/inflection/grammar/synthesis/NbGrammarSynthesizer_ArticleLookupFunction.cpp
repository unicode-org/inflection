/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NbGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NbGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

NbGrammarSynthesizer_ArticleLookupFunction::NbGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString)
    : super(model, includeSemanticValue, true)
    , singularMasculineString(singularMasculineString)
    , singularFeminineString(singularFeminineString)
    , singularNeuterString(singularNeuterString)
    , pluralString(pluralString)
    , countLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
    , genderLookupFunction(::inflection::util::LocaleUtils::NORWEGIAN(), {GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE(), GrammemeConstants::GENDER_NEUTER()})
{
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
}

inflection::dialog::SpeakableString* NbGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = NbGrammarSynthesizer::getNumber(displayValue.getFeatureValue(*npc(countFeature)));
    if (NbGrammarSynthesizer::Number::UNDEFINED == countValue) {
        ::std::unique_ptr<inflection::dialog::SpeakableString> countSpeakableString(countLookupFunction.getFeatureValue(displayValue, constraints));
        countValue = countSpeakableString != nullptr ? NbGrammarSynthesizer::getNumber(&countSpeakableString->getPrint()) : NbGrammarSynthesizer::Number::SINGULAR;
    }
    if (NbGrammarSynthesizer::Number::PLURAL == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    auto genderValue = NbGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (NbGrammarSynthesizer::Gender::UNDEFINED == genderValue) {
        ::std::unique_ptr<inflection::dialog::SpeakableString> genderSpeakableString(genderLookupFunction.getFeatureValue(displayValue, constraints));
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

} // namespace inflection::grammar::synthesis
