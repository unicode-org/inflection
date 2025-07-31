/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

SvGrammarSynthesizer_ArticleLookupFunction::SvGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString)
    : super(model, includeSemanticValue, true)
    , genderFeature(model.getFeature(GrammemeConstants::GENDER))
    , countFeature(model.getFeature(GrammemeConstants::NUMBER))
    , singularCommonString(singularCommonString)
    , singularNeuterString(singularNeuterString)
    , pluralString(pluralString)
    , countLookupFunction(::inflection::util::LocaleUtils::SWEDISH(), {GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
{
}

inflection::dialog::SpeakableString* SvGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = SvGrammarSynthesizer::getNumber(displayValue.getFeatureValue(*npc(countFeature)));
    if (SvGrammarSynthesizer::Number::undefined == countValue) {
        ::std::unique_ptr<inflection::dialog::SpeakableString> countSpeakableString(countLookupFunction.getFeatureValue(displayValue, constraints));
        countValue = countSpeakableString != nullptr ? SvGrammarSynthesizer::getNumber(&countSpeakableString->getPrint()) : SvGrammarSynthesizer::Number::singular;
    }
    if (SvGrammarSynthesizer::Number::plural == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    auto genderValue = SvGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (SvGrammarSynthesizer::Gender::undefined == genderValue) {
        ::std::unique_ptr<inflection::dialog::SpeakableString> genderSpeakableString(genderLookupFunction.getFeatureValue(displayValue, constraints));
        genderValue = genderSpeakableString != nullptr ? SvGrammarSynthesizer::getGender(&genderSpeakableString->getPrint()) : SvGrammarSynthesizer::Gender::common;
    }
    if (SvGrammarSynthesizer::Gender::neuter == genderValue) {
        return createPreposition(displayValue, singularNeuterString);
    }
    return createPreposition(displayValue, singularCommonString);
}

} // namespace inflection::grammar::synthesis
