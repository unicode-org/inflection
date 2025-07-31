/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/PtGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/PtGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

PtGrammarSynthesizer_ArticleLookupFunction::PtGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& defaultSingularString, const ::std::u16string& defaultPluralString, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& pluralMasculineString, const ::std::u16string& pluralFeminineString)
    : super(model, !derivedSemanticName.empty(), true)
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::PORTUGUESE())))
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::PORTUGUESE()))
    , derivedArticleFeature((!derivedSemanticName.empty() ? model.getFeature(derivedSemanticName) : static_cast< ::inflection::dialog::SemanticFeature* >(nullptr)))
    , countFeature(model.getFeature(GrammemeConstants::NUMBER))
    , genderFeature(model.getFeature(GrammemeConstants::GENDER))
    , countLookupFunction({GrammemeConstants::NUMBER_SINGULAR(), GrammemeConstants::NUMBER_PLURAL()})
    , genderLookupFunction({GrammemeConstants::GENDER_MASCULINE(), GrammemeConstants::GENDER_FEMININE()})
    , defaultString(defaultString)
    , defaultSingularString(defaultSingularString)
    , defaultPluralString(defaultPluralString)
    , singularMasculineString(singularMasculineString)
    , singularFeminineString(singularFeminineString)
    , pluralMasculineString(pluralMasculineString)
    , pluralFeminineString(pluralFeminineString)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {GrammemeConstants::GENDER_FEMININE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {GrammemeConstants::GENDER_MASCULINE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {GrammemeConstants::NUMBER_SINGULAR()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL()}));
}

PtGrammarSynthesizer_ArticleLookupFunction::PtGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const PtGrammarSynthesizer_ArticleLookupFunction& other)
    : PtGrammarSynthesizer_ArticleLookupFunction(model, derivedSemanticName, other.defaultString, other.defaultSingularString, other.defaultPluralString, other.singularMasculineString, other.singularFeminineString, other.pluralMasculineString, other.pluralFeminineString)
{
}


PtGrammarSynthesizer_ArticleLookupFunction::~PtGrammarSynthesizer_ArticleLookupFunction()
{
}

inflection::dialog::SpeakableString* PtGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    const auto &displayString = displayValue.getDisplayString();
    auto countValue = PtGrammarSynthesizer::getNumber(displayValue.getFeatureValue(*npc(countFeature)));
    if (countValue == PtGrammarSynthesizer::Number::undefined) {
        auto value(countLookupFunction.determine(displayString));
        countValue = PtGrammarSynthesizer::getNumber(&value);
    }
    auto genderValue = PtGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (genderValue == PtGrammarSynthesizer::Gender::undefined) {
        auto value(genderLookupFunction.determine(displayString));
        genderValue = PtGrammarSynthesizer::getGender(&value);
    }
    if (PtGrammarSynthesizer::Number::singular == countValue && PtGrammarSynthesizer::Gender::masculine == genderValue) {
        return createPreposition(displayValue, singularMasculineString);
    }
    if (PtGrammarSynthesizer::Number::singular == countValue && PtGrammarSynthesizer::Gender::feminine == genderValue) {
        return createPreposition(displayValue, singularFeminineString);
    }
    if (PtGrammarSynthesizer::Number::plural == countValue && PtGrammarSynthesizer::Gender::masculine == genderValue) {
        return createPreposition(displayValue, pluralMasculineString);
    }
    if (PtGrammarSynthesizer::Number::plural == countValue && PtGrammarSynthesizer::Gender::feminine == genderValue) {
        return createPreposition(displayValue, pluralFeminineString);
    }
    if (PtGrammarSynthesizer::Number::singular == countValue) {
        return createPreposition(displayValue, defaultSingularString);
    }
    if (PtGrammarSynthesizer::Number::plural == countValue) {
        return createPreposition(displayValue, defaultPluralString);
    }
    return createPreposition(displayValue, defaultString);
}

} // namespace inflection::grammar::synthesis
