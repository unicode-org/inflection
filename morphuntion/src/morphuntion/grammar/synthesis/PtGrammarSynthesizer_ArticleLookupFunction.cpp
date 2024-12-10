/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

PtGrammarSynthesizer_ArticleLookupFunction::PtGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& defaultSingularString, const ::std::u16string& defaultPluralString, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& pluralMasculineString, const ::std::u16string& pluralFeminineString)
    : super(model, !derivedSemanticName.empty(), true)
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::PORTUGUESE())))
    , tokenizer(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::PORTUGUESE()))
    , derivedArticleFeature((!derivedSemanticName.empty() ? model.getFeature(derivedSemanticName) : static_cast< ::morphuntion::dialog::SemanticFeature* >(nullptr)))
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
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {GrammemeConstants::GENDER_FEMININE()}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {GrammemeConstants::GENDER_MASCULINE()}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionarySingular, {GrammemeConstants::NUMBER_SINGULAR()}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {GrammemeConstants::NUMBER_PLURAL()}));
}

PtGrammarSynthesizer_ArticleLookupFunction::PtGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const PtGrammarSynthesizer_ArticleLookupFunction& other)
    : PtGrammarSynthesizer_ArticleLookupFunction(model, derivedSemanticName, other.defaultString, other.defaultSingularString, other.defaultPluralString, other.singularMasculineString, other.singularFeminineString, other.pluralMasculineString, other.pluralFeminineString)
{
}


PtGrammarSynthesizer_ArticleLookupFunction::~PtGrammarSynthesizer_ArticleLookupFunction()
{
}

morphuntion::dialog::SpeakableString* PtGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    const auto &displayString = displayValue.getDisplayString();
    auto countValue = PtGrammarSynthesizer::getCount(displayValue.getFeatureValue(*npc(countFeature)));
    if (countValue == PtGrammarSynthesizer::Count::undefined) {
        auto value(countLookupFunction.determine(displayString));
        countValue = PtGrammarSynthesizer::getCount(&value);
    }
    auto genderValue = PtGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (genderValue == PtGrammarSynthesizer::Gender::undefined) {
        auto value(genderLookupFunction.determine(displayString));
        genderValue = PtGrammarSynthesizer::getGender(&value);
    }
    if (PtGrammarSynthesizer::Count::singular == countValue && PtGrammarSynthesizer::Gender::masculine == genderValue) {
        return createPreposition(displayValue, singularMasculineString);
    }
    if (PtGrammarSynthesizer::Count::singular == countValue && PtGrammarSynthesizer::Gender::feminine == genderValue) {
        return createPreposition(displayValue, singularFeminineString);
    }
    if (PtGrammarSynthesizer::Count::plural == countValue && PtGrammarSynthesizer::Gender::masculine == genderValue) {
        return createPreposition(displayValue, pluralMasculineString);
    }
    if (PtGrammarSynthesizer::Count::plural == countValue && PtGrammarSynthesizer::Gender::feminine == genderValue) {
        return createPreposition(displayValue, pluralFeminineString);
    }
    if (PtGrammarSynthesizer::Count::singular == countValue) {
        return createPreposition(displayValue, defaultSingularString);
    }
    if (PtGrammarSynthesizer::Count::plural == countValue) {
        return createPreposition(displayValue, defaultPluralString);
    }
    return createPreposition(displayValue, defaultString);
}

} // namespace morphuntion::grammar::synthesis
