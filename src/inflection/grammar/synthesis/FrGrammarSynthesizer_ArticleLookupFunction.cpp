/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

FrGrammarSynthesizer_ArticleLookupFunction::FrGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model,
                                               const ::inflection::dialog::DictionaryLookupFunction& numberLookupFunction,
                                               const ::inflection::dialog::DictionaryLookupFunction& genderLookupFunction,
                                               bool insertSpace,
                                               bool applyContraction,
                                               bool applyVowelElisionToBothGenders,
                                               const char16_t* derivedSemanticName,
                                               std::u16string_view defaultString,
                                               std::u16string_view defaultStartsWithVowelString,
                                               std::u16string_view singularMasculineString,
                                               std::u16string_view singularFeminineString,
                                               std::u16string_view singularStartsWithVowelString,
                                               std::u16string_view pluralString)
    : super(model, derivedSemanticName != nullptr, insertSpace)
    , tokenizer(applyContraction ? ::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::FRENCH()) : nullptr)
    , numberLookupFunction(numberLookupFunction)
    , genderLookupFunction(genderLookupFunction)
    , defaultString(defaultString)
    , defaultStartsWithVowelString(defaultStartsWithVowelString)
    , singularMasculineString(singularMasculineString)
    , singularFeminineString(singularFeminineString)
    , singularStartsWithVowelString(singularStartsWithVowelString)
    , pluralString(pluralString)
    , applyContraction(applyContraction)
    , applyVowelElisionToBothGenders(applyVowelElisionToBothGenders)
{
    this->derivedArticleFeature = (derivedSemanticName != nullptr ? model.getFeature(npc(derivedSemanticName)) : nullptr);
    this->numberFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
}

FrGrammarSynthesizer_ArticleLookupFunction::FrGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const FrGrammarSynthesizer_ArticleLookupFunction& other)
    : FrGrammarSynthesizer_ArticleLookupFunction(model, other.numberLookupFunction, other.genderLookupFunction, insertSpace, other.applyContraction, other.applyVowelElisionToBothGenders, derivedSemanticName, other.defaultString, other.defaultStartsWithVowelString, other.singularMasculineString, other.singularFeminineString, other.singularStartsWithVowelString, other.pluralString)
{
}

FrGrammarSynthesizer_ArticleLookupFunction::~FrGrammarSynthesizer_ArticleLookupFunction()
{
}

inflection::dialog::SpeakableString* FrGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    const auto& displayString = displayValue.getDisplayString();
    if (applyContraction && !displayString.empty()) {
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
        auto firstToken = npc(tokenChain->getHead())->getNext();
        const auto& firstWord = npc(firstToken)->getCleanValue();
        if (firstWord == u"le") {
            return new inflection::dialog::SpeakableString(inflection::util::StringViewUtils::trim(std::u16string(singularMasculineString) + (displayString.length() > firstWord.length() ? displayString.substr(npc(firstToken)->getEndChar() + 1) : u"")));
        }
        else if (firstWord == u"les") {
            return new inflection::dialog::SpeakableString(inflection::util::StringViewUtils::trim(std::u16string(pluralString) + (displayString.length() > firstWord.length() ? displayString.substr(npc(firstToken)->getEndChar() + 1) : u"")));
        }
    }
    auto countValue = FrGrammarSynthesizer::getNumber(displayValue.getFeatureValue(*npc(numberFeature)));
    if (countValue == FrGrammarSynthesizer::Number::undefined) {
        auto value(numberLookupFunction.determine(displayString));
        countValue = FrGrammarSynthesizer::getNumber(&value);
    }
    auto genderValue = FrGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (genderValue == FrGrammarSynthesizer::Gender::undefined) {
        auto value(genderLookupFunction.determine(displayString));
        genderValue = FrGrammarSynthesizer::getGender(&value);
    }
    if (FrGrammarSynthesizer::Number::plural == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    if (FrGrammarSynthesizer::Number::singular == countValue) {
        if (!singularStartsWithVowelString.empty() && !displayString.empty()
            && (applyVowelElisionToBothGenders || FrGrammarSynthesizer::Gender::masculine == genderValue)
            && ::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), displayString))
        {
            return createPreposition(displayValue, singularStartsWithVowelString);
        }
        if (FrGrammarSynthesizer::Gender::masculine == genderValue) {
            return createPreposition(displayValue, singularMasculineString);
        }
        if (FrGrammarSynthesizer::Gender::feminine == genderValue) {
            return createPreposition(displayValue, singularFeminineString);
        }
    }
    if (!defaultStartsWithVowelString.empty() && !displayString.empty() && ::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), displayString)) {
        return createPreposition(displayValue, defaultStartsWithVowelString);
    }
    return createPreposition(displayValue, defaultString);
}


} // namespace inflection::grammar::synthesis
