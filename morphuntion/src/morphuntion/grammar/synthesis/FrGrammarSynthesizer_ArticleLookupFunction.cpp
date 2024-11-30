/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/Token_Head.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

FrGrammarSynthesizer_ArticleLookupFunction::FrGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool insertSpace, bool applyContraction, bool applyVowelElisionToBothGenders, const char16_t* derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& defaultStartsWithVowelString, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& singularStartsWithVowelString, const ::std::u16string& pluralString)
    : super(model, derivedSemanticName != nullptr, insertSpace)
    , tokenizer(applyContraction ? ::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::FRENCH()) : nullptr)
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
    this->countFeature = model.getFeature(GrammemeConstants::NUMBER);
    this->genderFeature = model.getFeature(GrammemeConstants::GENDER);
}

FrGrammarSynthesizer_ArticleLookupFunction::FrGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const FrGrammarSynthesizer_ArticleLookupFunction& other)
    : FrGrammarSynthesizer_ArticleLookupFunction(model, insertSpace, other.applyContraction, other.applyVowelElisionToBothGenders, derivedSemanticName, other.defaultString, other.defaultStartsWithVowelString, other.singularMasculineString, other.singularFeminineString, other.singularStartsWithVowelString, other.pluralString)
{
}

FrGrammarSynthesizer_ArticleLookupFunction::~FrGrammarSynthesizer_ArticleLookupFunction()
{
}

morphuntion::dialog::SpeakableString* FrGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    const auto& displayString = displayValue.getDisplayString();
    if (applyContraction && !displayString.empty()) {
        ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
        auto firstToken = npc(tokenChain->getHead())->getNext();
        const auto& firstWord = npc(firstToken)->getCleanValue();
        if (firstWord == u"le") {
            return new morphuntion::dialog::SpeakableString(std::u16string(morphuntion::util::StringViewUtils::trim(singularMasculineString + (displayString.length() > firstWord.length() ? displayString.substr(npc(firstToken)->getEndChar() + 1) : u""))));
        }
        else if (firstWord == u"les") {
            return new morphuntion::dialog::SpeakableString(std::u16string(morphuntion::util::StringViewUtils::trim(pluralString + (displayString.length() > firstWord.length() ? displayString.substr(npc(firstToken)->getEndChar() + 1) : u""))));
        }
    }
    auto countValue = FrGrammarSynthesizer::getCount(displayValue.getFeatureValue(*npc(countFeature)));
    if (countValue == FrGrammarSynthesizer::Count::undefined) {
        auto value(countLookupFunction.determine(displayString));
        countValue = FrGrammarSynthesizer::getCount(&value);
    }
    auto genderValue = FrGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    if (genderValue == FrGrammarSynthesizer::Gender::undefined) {
        auto value(genderLookupFunction.determine(displayString));
        genderValue = FrGrammarSynthesizer::getGender(&value);
    }
    if (FrGrammarSynthesizer::Count::plural == countValue) {
        return createPreposition(displayValue, pluralString);
    }
    if (FrGrammarSynthesizer::Count::singular == countValue) {
        if (!singularStartsWithVowelString.empty() && !displayString.empty()
            && (applyVowelElisionToBothGenders || FrGrammarSynthesizer::Gender::masculine == genderValue)
            && ::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), displayString))
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
    if (!defaultStartsWithVowelString.empty() && !displayString.empty() && ::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), displayString)) {
        return createPreposition(displayValue, defaultStartsWithVowelString);
    }
    return createPreposition(displayValue, defaultString);
}


} // namespace morphuntion::grammar::synthesis
