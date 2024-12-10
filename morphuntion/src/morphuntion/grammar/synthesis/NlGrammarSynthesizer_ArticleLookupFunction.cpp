/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/tokenizer/Token_Delim.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Tokenizer.hpp>
#include <morphuntion/tokenizer/TokenizerFactory.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_ArticleLookupFunction::NlGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& defaultString, const ::std::u16string& singularNeuterString)
    : super(model, includeSemanticValue, true)
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(::morphuntion::util::LocaleUtils::DUTCH())))
    , tokenizer(npc(::morphuntion::tokenizer::TokenizerFactory::createTokenizer(::morphuntion::util::LocaleUtils::DUTCH())))
    , countFeature(npc(model.getFeature(GrammemeConstants::NUMBER)))
    , genderFeature(npc(model.getFeature(GrammemeConstants::GENDER)))
    , sizeFeature(npc(model.getFeature(NlGrammarSynthesizer::SIZENESS())))
    , defaultString(defaultString)
    , singularNeuterString(singularNeuterString)
{
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {u"feminine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {u"masculine"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {u"neuter"}));
    dictionaryGenderMask = dictionaryFeminine | dictionaryMasculine | dictionaryNeuter;
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryPlural, {u"plural"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryDiminutive, {u"diminutive"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {u"noun"}));
    ::morphuntion::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {u"adjective"}));
}

NlGrammarSynthesizer_ArticleLookupFunction::~NlGrammarSynthesizer_ArticleLookupFunction()
{
}

morphuntion::dialog::SpeakableString* NlGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    auto countValue = NlGrammarSynthesizer::getCount(displayValue.getFeatureValue(*npc(countFeature)));
    auto genderValue = NlGrammarSynthesizer::getGender(displayValue.getFeatureValue(*npc(genderFeature)));
    auto diminutiveValue = displayValue.getFeatureValue(*npc(sizeFeature));
    auto isDiminutive = diminutiveValue != nullptr && NlGrammarSynthesizer::SIZENESS_DIMINUTIVE() == *npc(diminutiveValue);
    if (countValue == NlGrammarSynthesizer::Count::undefined || genderValue == NlGrammarSynthesizer::Gender::undefined) {
        const auto& displayString = displayValue.getDisplayString();
        int64_t phraseType = 0;
        if (dictionary.getCombinedBinaryType(&phraseType, displayString) == nullptr) {
            auto significantNoun = determineSignificantNoun(displayString);
            if (!significantNoun.empty()) {
                dictionary.getCombinedBinaryType(&phraseType, significantNoun);
            }
        }
        if (phraseType != 0) {
            if (countValue == NlGrammarSynthesizer::Count::undefined) {
                if ((phraseType & dictionaryPlural) != 0) {
                    countValue = NlGrammarSynthesizer::Count::plural;
                } else {
                    countValue = NlGrammarSynthesizer::Count::singular;
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
    if (NlGrammarSynthesizer::Count::singular == countValue && (NlGrammarSynthesizer::Gender::neuter == genderValue || isDiminutive)) {
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
    ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(npc(tokenizer.get())->createTokenChain(phrase));
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

} // namespace morphuntion::grammar::synthesis
