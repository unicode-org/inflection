/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_ArticleLookupFunction::NlGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& defaultString, const ::std::u16string& singularNeuterString)
    : super(model, includeSemanticValue, true)
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::DUTCH())))
    , tokenizer(npc(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::DUTCH())))
    , countFeature(*npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(*npc(model.getFeature(GrammemeConstants::GENDER)))
    , sizeFeature(*npc(model.getFeature(GrammemeConstants::SIZENESS)))
    , defaultString(defaultString)
    , singularNeuterString(singularNeuterString)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {u"neuter"}));
    dictionaryGenderMask = dictionaryFeminine | dictionaryMasculine | dictionaryNeuter;
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryDiminutive, {u"diminutive"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
}

NlGrammarSynthesizer_ArticleLookupFunction::~NlGrammarSynthesizer_ArticleLookupFunction()
{
}

inflection::dialog::SpeakableString* NlGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    auto countValue = NlGrammarSynthesizer::getNumber(getDisplayFeatureValue(displayValue, countFeature));
    auto genderValue = NlGrammarSynthesizer::getGender(getDisplayFeatureValue(displayValue, genderFeature));
    auto diminutiveValue = displayValue.getFeatureValue(sizeFeature);
    auto isDiminutive = diminutiveValue != nullptr && GrammemeConstants::SIZENESS_DIMINUTIVE() == *npc(diminutiveValue);
    if (countValue == NlGrammarSynthesizer::Number::undefined || genderValue == NlGrammarSynthesizer::Gender::undefined) {
        const auto& displayString = displayValue.getDisplayString();
        int64_t phraseType = 0;
        if (dictionary.getCombinedBinaryType(&phraseType, displayString) == nullptr) {
            auto significantNoun = determineSignificantNoun(displayString);
            if (!significantNoun.empty()) {
                dictionary.getCombinedBinaryType(&phraseType, significantNoun);
            }
        }
        if (phraseType != 0) {
            if (countValue == NlGrammarSynthesizer::Number::undefined) {
                if ((phraseType & dictionaryPlural) != 0) {
                    countValue = NlGrammarSynthesizer::Number::plural;
                } else {
                    countValue = NlGrammarSynthesizer::Number::singular;
                }
            }
            if (genderValue == NlGrammarSynthesizer::Gender::undefined) {
                if ((phraseType & dictionaryGenderMask) == dictionaryFeminine) {
                    genderValue = NlGrammarSynthesizer::Gender::feminine;
                } else if ((phraseType & dictionaryGenderMask) == dictionaryMasculine) {
                    genderValue = NlGrammarSynthesizer::Gender::masculine;
                } else if ((phraseType & dictionaryGenderMask) == dictionaryNeuter) {
                    genderValue = NlGrammarSynthesizer::Gender::neuter;
                }
            }
            if (!isDiminutive) {
                isDiminutive = this->isDiminutive(displayString, phraseType);
            }
        }
    }
    if (NlGrammarSynthesizer::Number::singular == countValue && (NlGrammarSynthesizer::Gender::neuter == genderValue || isDiminutive)) {
        return createPreposition(displayValue, singularNeuterString);
    }
    return createPreposition(displayValue, defaultString);
}

bool NlGrammarSynthesizer_ArticleLookupFunction::isDiminutive(const ::std::u16string& word, int64_t phraseType) const
{
    if ((phraseType & dictionaryDiminutive) != 0) {
        return true;
    }
    ::std::u16string guess;
    NlGrammarSynthesizer::guessDiminutive(&guess, word);
    if (guess.empty()) {
        return false;
    }
    return dictionary.isKnownWord(guess);
}

::std::u16string NlGrammarSynthesizer_ArticleLookupFunction::determineSignificantNoun(const ::std::u16string& phrase) const
{
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(tokenizer.get())->createTokenChain(phrase));
    auto tokens = GrammarSynthesizerUtil::getSignificantWords(*tokenChain);
    if (tokens.empty()) {
        return {};
    }
    int32_t i = 0;
    int32_t tokensSize = int32_t(tokens.size());
    while (i < tokensSize && dictionary.hasAllProperties(tokens[i], dictionaryAdjective)) {
        i++;
    }
    while (i < tokensSize &&
           (dictionary.hasAllProperties(tokens[i], dictionaryNoun) || fugenElements.contains(tokens[i])))
    {
        i++;
    }
    if (i > 0 && dictionary.hasAllProperties(tokens[i-1], dictionaryNoun)) {
        return tokens[i-1];
    }
    return tokens[tokens.size()-1];
}

} // namespace inflection::grammar::synthesis
