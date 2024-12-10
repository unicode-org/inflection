/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

SvGrammarSynthesizer_ArticleLookupFunction::SvGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString)
    : super(model, includeSemanticValue, true)
    , genderFeature(model.getFeature(GrammemeConstants::GENDER))
    , countFeature(model.getFeature(GrammemeConstants::NUMBER))
    , singularCommonString(singularCommonString)
    , singularNeuterString(singularNeuterString)
    , pluralString(pluralString)
    , countLookupFunction(::morphuntion::util::LocaleUtils::SWEDISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
{
}

morphuntion::dialog::SpeakableString* SvGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = SvGrammarSynthesizer::getCount(displayValue.getFeatureValue(*npc(countFeature)));
    if (SvGrammarSynthesizer::Count::undefined == countValue) {
        ::std::unique_ptr<morphuntion::dialog::SpeakableString> countSpeakableString(countLookupFunction.getFeatureValue(displayValue, constraints));
        countValue = countSpeakableString != nullptr ? SvGrammarSynthesizer::getCount(&countSpeakableString->getPrint()) : SvGrammarSynthesizer::Count::singular;
    }
    if (SvGrammarSynthesizer::Count::plural == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    auto genderValue = SvGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (SvGrammarSynthesizer::Gender::undefined == genderValue) {
        ::std::unique_ptr<morphuntion::dialog::SpeakableString> genderSpeakableString(genderLookupFunction.getFeatureValue(displayValue, constraints));
        genderValue = genderSpeakableString != nullptr ? SvGrammarSynthesizer::getGender(&genderSpeakableString->getPrint()) : SvGrammarSynthesizer::Gender::common;
    }
    if (SvGrammarSynthesizer::Gender::neuter == genderValue) {
        return createPreposition(displayValue, singularNeuterString);
    }
    return createPreposition(displayValue, singularCommonString);
}

} // namespace morphuntion::grammar::synthesis
