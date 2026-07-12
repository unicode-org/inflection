/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NbGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NbGrammarSynthesizer.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

NbGrammarSynthesizer_ArticleLookupFunction::NbGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model,
                                               const ::inflection::dialog::DictionaryLookupFunction& numberLookupFunction,
                                               const ::inflection::dialog::DictionaryLookupFunction& genderLookupFunction,
                                               bool includeSemanticValue,
                                               std::u16string_view singularMasculineString,
                                               std::u16string_view singularFeminineString,
                                               std::u16string_view singularNeuterString,
                                               std::u16string_view pluralString)
    : super(model, includeSemanticValue, true)
    , singularMasculineString(singularMasculineString)
    , singularFeminineString(singularFeminineString)
    , singularNeuterString(singularNeuterString)
    , pluralString(pluralString)
    , numberLookupFunction(numberLookupFunction)
    , genderLookupFunction(genderLookupFunction)
{
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
    this->numberFeature = model.getFeature(GrammemeConstants::NUMBER);
}

inflection::dialog::SpeakableString* NbGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = NbGrammarSynthesizer::getNumber(displayValue.getFeatureValue(*npc(numberFeature)));
    if (NbGrammarSynthesizer::Number::UNDEFINED == countValue) {
        ::std::unique_ptr<inflection::dialog::SpeakableString> countSpeakableString(numberLookupFunction.getFeatureValue(displayValue, constraints));
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
