/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlDisplayFunction.hpp>

#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlDisplayFunction::NlGrammarSynthesizer_NlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(model.getLocale()))
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(model.getLocale())))
    , adjectiveInflector(model)
    , nounInflector(model)
    , verbInflector(model)
    , definiteArticleLookupFunction(model, true, u"de", u"het")
    , indefiniteArticleLookupFunction(model, nullptr, u"een")
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, NlGrammarSynthesizer::DE, &indefiniteArticleLookupFunction, NlGrammarSynthesizer::ARTICLE_INDEFINITE)
{
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryFeminine, {GrammemeConstants::GENDER_FEMININE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryMasculine, {GrammemeConstants::GENDER_MASCULINE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNeuter, {GrammemeConstants::GENDER_NEUTER()}));
    dictionaryGenderMask = dictionaryFeminine | dictionaryMasculine | dictionaryNeuter;
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryAdjective, {GrammemeConstants::POS_ADJECTIVE()}));
    ::inflection::util::Validate::notNull(dictionary.getBinaryProperties(&dictionaryNoun, {GrammemeConstants::POS_NOUN()}));
}

NlGrammarSynthesizer_NlDisplayFunction::~NlGrammarSynthesizer_NlDisplayFunction()
{
}

::inflection::dialog::DisplayValue * NlGrammarSynthesizer_NlDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    if (displayString.empty()) {
        return nullptr;
    }
    if (!constraints.empty()) {
        int64_t wordGrammemes = 0;
        dictionary.getCombinedBinaryType(&wordGrammemes, displayString);

        ::std::u16string inflectedString;
        if (!(inflectedString = nounInflector.inflect(displayString, wordGrammemes, constraints)).empty()
            || !(inflectedString = adjectiveInflector.inflect(displayString, wordGrammemes, constraints)).empty()
            || !(inflectedString = verbInflector.inflect(displayString, wordGrammemes, constraints)).empty()
            || (wordGrammemes == 0 && !(inflectedString = inflectPhrase(displayString, constraints)).empty()))
        {
            displayString = inflectedString;
        }
    }
    return definitenessDisplayFunction.addDefiniteness(new ::inflection::dialog::DisplayValue(displayString, constraints), constraints);
}

::std::u16string NlGrammarSynthesizer_NlDisplayFunction::inflectPhrase(const ::std::u16string& displayString, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(npc(tokenizer.get())->createTokenChain(displayString)));
    bool isLastNoun = false;
    bool hasAdjectives = false;
    int64_t posMask = dictionaryNoun | dictionaryAdjective;
    for (const auto& token : *tokenChain) {
        if (token.isSignificant()) {
            int64_t wordGrammemes = 0;
            if (dictionary.getCombinedBinaryType(&wordGrammemes, token.getValue()) == nullptr || (wordGrammemes & posMask) == 0) {
                // Unknown word or wrong part of speech
                return {};
            }
            if ((wordGrammemes & dictionaryAdjective) == dictionaryAdjective) {
                hasAdjectives = true;
                if (isLastNoun) {
                    // adjective, noun, adjective
                    return {};
                }
                // So far, so good.
            }
            else if ((wordGrammemes & dictionaryNoun) == dictionaryNoun) {
                if (!isLastNoun) {
                    isLastNoun = true;
                }
                else {
                    // multiple nouns
                    return {};
                }
            }
        }
    }

    if (isLastNoun) {
        return GrammarSynthesizerUtil::inflectSignificantWords(constraints, *tokenChain, *this);
    }
    if (hasAdjectives) {
        return GrammarSynthesizerUtil::inflectSignificantWords(constraints, *tokenChain, adjectiveInflector);
    }
    // Not enough words.
    return {};
}

::std::vector<::std::u16string> NlGrammarSynthesizer_NlDisplayFunction::inflectSignificantTokens(const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints, ::std::vector<::std::u16string>* tokens, const ::std::vector<int32_t>& indexesOfSignificant) const
{
    auto headWordIndex = indexesOfSignificant.back();
    const auto& headWord = (*tokens)[headWordIndex];
    int64_t headWordGrammemes = 0;
    dictionary.getCombinedBinaryType(&headWordGrammemes, headWord);
    npc(tokens)->at(headWordIndex) = nounInflector.inflect(headWord, headWordGrammemes, constraints);
    if ((*tokens)[headWordIndex].empty()) {
        return {};
    }
    auto targetCount = nounInflector.getGrammaticalNumber(constraints);
    auto gender = getGender(headWordGrammemes);
    auto targetDeclension = getAdjectiveDeclension(targetCount, gender);
    int32_t indexesOfSignificantLimit = int32_t(indexesOfSignificant.size() - 1);
    for (int32_t j = 0; j < indexesOfSignificantLimit; j++) {
        auto index = indexesOfSignificant[j];
        auto inflectedToken = NlGrammarSynthesizer_NlAdjectiveInflectionPattern::inflectWithDeclension((*tokens)[index], targetDeclension);
        if (inflectedToken.empty()) {
            return {};
        }
        npc(tokens)->at(index) = inflectedToken;
    }
    return *npc(tokens);
}

NlGrammarSynthesizer::Declension NlGrammarSynthesizer_NlDisplayFunction::getAdjectiveDeclension(NlGrammarSynthesizer::Number nounCount, NlGrammarSynthesizer::Gender nounGender)
{
    if (nounCount == NlGrammarSynthesizer::Number::undefined) {
        return NlGrammarSynthesizer::Declension::undefined;
    }
    else if (nounCount == NlGrammarSynthesizer::Number::plural) {
        return NlGrammarSynthesizer::Declension::declined;
    }
    else if (nounCount == NlGrammarSynthesizer::Number::singular) {
        if (nounGender == NlGrammarSynthesizer::Gender::undefined) {
            return NlGrammarSynthesizer::Declension::undefined;
        }
        else if (nounGender == NlGrammarSynthesizer::Gender::neuter) {
            return NlGrammarSynthesizer::Declension::undeclined;
        }
        return NlGrammarSynthesizer::Declension::declined;
    }

    return NlGrammarSynthesizer::Declension::undefined;
}

NlGrammarSynthesizer::Gender NlGrammarSynthesizer_NlDisplayFunction::getGender(int64_t wordGrammemes) const
{
    if ((wordGrammemes & dictionaryGenderMask) == dictionaryFeminine) {
        return NlGrammarSynthesizer::Gender::feminine;
    } else if ((wordGrammemes & dictionaryGenderMask) == dictionaryMasculine) {
        return NlGrammarSynthesizer::Gender::masculine;
    } else if ((wordGrammemes & dictionaryGenderMask) == dictionaryNeuter) {
        return NlGrammarSynthesizer::Gender::neuter;
    }
    return NlGrammarSynthesizer::Gender::undefined;
}

} // namespace inflection::grammar::synthesis
