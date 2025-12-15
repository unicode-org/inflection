/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/npc.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/util/Validate.hpp>
#include "inflection/tokenizer/TokenChain.hpp"
#include "inflection/tokenizer/Token_Word.hpp"
#include "inflection/tokenizer/TokenizerFactory.hpp"

namespace inflection::dialog {

DictionaryLookupFunction::DictionaryLookupFunction(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string>& tags)
    : super()
    , dictionaryMorphology(locale)
    , tokenizer(npc(inflection::tokenizer::TokenizerFactory::createTokenizer(locale)))
{
    ::inflection::util::Validate::notNull(dictionaryMorphology.getDictionary().getBinaryProperties(&mask, tags));
}

DictionaryLookupFunction::DictionaryLookupFunction(const util::ULocale &locale, const std::vector<::std::u16string> &tags, const std::vector<::std::u16string> &disambiguationPartsOfSpeech)
    : super()
    , dictionaryMorphology(locale)
    , tokenizer(npc(inflection::tokenizer::TokenizerFactory::createTokenizer(locale)))
    , disambiguationPartsOfSpeech(convertDisambiguationPartsOfSpeech(dictionaryMorphology.getDictionary(), disambiguationPartsOfSpeech))
    , enableDisambiguation(true)
{
    ::inflection::util::Validate::notNull(dictionaryMorphology.getDictionary().getBinaryProperties(&mask, tags));
}

::std::vector<int64_t> DictionaryLookupFunction::convertDisambiguationPartsOfSpeech(const dictionary::DictionaryMetaData &dictionary, const ::std::vector<::std::u16string> &disambiguationPartsOfSpeech) {
    ::std::vector<int64_t> result;
    result.reserve(disambiguationPartsOfSpeech.size());
    for (const auto &partOfSpeech : disambiguationPartsOfSpeech) {
        result.push_back(dictionary.getBinaryProperties({partOfSpeech}));
    }
    return result;
}

::std::u16string DictionaryLookupFunction::determineWithPropertyMask(int64_t properties) const {
    return getDictionary().getPropertyName(properties & mask);
}

::std::u16string DictionaryLookupFunction::determine(const ::std::u16string& word) const
{
    int64_t properties = 0;
    getDictionary().getCombinedBinaryType(&properties, word);
    if (enableDisambiguation && std::popcount(static_cast<uint64_t>(properties)) != 1) {
        // OK so now it's either ambiguous or not in the dictionary.
        return determineWithDisambiguation(word);
    }

    return determineWithPropertyMask(properties);
}
::std::u16string DictionaryLookupFunction::determinePhrase(const ::std::u16string& word, bool firstWordDeterminesValue) const
{
    int64_t properties = 0;
    getDictionary().getCombinedBinaryType(&properties, word);
    if (std::popcount(static_cast<uint64_t>(properties)) != 1) {
        // OK so now it's either ambiguous or not in the dictionary.
        ::std::u16string result;
        if (properties == 0) {
            // It's not in the dictionary.
            ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(npc(tokenizer->createTokenChain(word)));
            if (tokenChain->getWordCount() > 1) {
                // It's an unknown phrase.
                const ::inflection::tokenizer::Token_Word *firstWordToken = nullptr;
                const ::inflection::tokenizer::Token_Word *firstRelevantToken = nullptr;
                getImportantTokens(tokenChain.get(), firstWordDeterminesValue, firstWordToken, firstRelevantToken);
                if (firstRelevantToken != nullptr) {
                    // Try the first relevant token.
                    result = DictionaryLookupFunction::determine(npc(firstRelevantToken)->getValue());
                }
                if (result.empty() && firstWordToken != nullptr) {
                    // That didn't go well. Try the first word token (excluding delimiters).
                    result = DictionaryLookupFunction::determine(npc(firstWordToken)->getValue());
                }
                if (result.empty()) {
                    return DictionaryLookupFunction::determine(word);
                }
            }
            // else it's an unknown single word or not a word.
        }
        else {
            // It's a known single word. Try to disambiguate it.
            result = determineWithDisambiguation(word);
        }

        return result;
    }

    return determineWithPropertyMask(properties);
}

void DictionaryLookupFunction::getImportantTokens(const ::inflection::tokenizer::TokenChain *tokenChain,
                                                  bool firstWordDeterminesValue,
                                                  const inflection::tokenizer::Token_Word *& firstWordToken,
                                                  const inflection::tokenizer::Token_Word *& firstRelevantToken) const
{
    int64_t properties = 0;
    for (const auto &token: *npc(tokenChain)) {
        if (dynamic_cast< const inflection::tokenizer::Token_Word * >(&token) != nullptr) {
            if (firstWordToken == nullptr) {
                firstWordToken = dynamic_cast< const inflection::tokenizer::Token_Word * >(&token);
            }
            if (getDictionary().getCombinedBinaryType(&properties, token.getCleanValue()) != nullptr
                && (properties & mask) != 0)
            {
                firstRelevantToken = dynamic_cast< const inflection::tokenizer::Token_Word * >(&token);
                if (firstWordDeterminesValue) {
                    break;
                }
            }
        }
    }
}

SpeakableString* DictionaryLookupFunction::getFeatureValue(const DisplayValue& displayValue, const ::std::map<SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    auto result = determine(displayString);
    if (result.empty()) {
        return nullptr;
    }
    return new SpeakableString(result);
}

const dictionary::DictionaryMetaData& DictionaryLookupFunction::getDictionary() const
{
    return dictionaryMorphology.getDictionary();
}

::std::u16string DictionaryLookupFunction::determineWithDisambiguation(std::u16string_view word) const {
    ::std::vector<int64_t> propertySets = dictionaryMorphology.getWordGrammemeSets(word);
    if (propertySets.empty()) {
        return {};
    }
    ::std::vector<int64_t> disambiguatedPropertySets(disambiguationPartsOfSpeech.size() + 1, 0);
    for (const auto properties : propertySets) {
        int64_t i = 0;
        for (const auto partOfSpeech : disambiguationPartsOfSpeech) {
            if ((partOfSpeech & properties) != 0) {
                disambiguatedPropertySets[i] |= properties;
                break;
            }
            i += 1;
        }
        if (i == static_cast<int64_t>(disambiguationPartsOfSpeech.size())) {
            disambiguatedPropertySets[i] |= properties;
        }
    }
    for (const auto disambiguatedProperties : disambiguatedPropertySets) {
        if (disambiguatedProperties != 0) {
            return determineWithPropertyMask(disambiguatedProperties);
        }
    }
    return {};
}

::std::u16string DictionaryLookupFunction::getFirstWord(const ::std::u16string& word) const
{
    std::unique_ptr<inflection::tokenizer::TokenChain> tokenChain(npc(tokenizer->createTokenChain(word)));
    for (const auto& token : *tokenChain) {
        if (dynamic_cast< const ::inflection::tokenizer::Token_Word* >(&token) != nullptr) {
            return token.getCleanValue();
        }
    }
    return {};
}

} // namespace inflection::dialog
