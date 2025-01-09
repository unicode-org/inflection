/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/metadata/fwd.hpp>
#include "Dictionary.hpp"
#include "DictionaryLogger.hpp"
#include <cstdint>
#include <map>
#include <vector>
#include <string_view>
#include <ostream>

class LexicalDictionaryBuilder final
{
public:
    static void writeDictionary(::std::ofstream& writer, DictionaryLogger& logger, const Dictionary& dictionary, const ::std::string& sourceInflectionFilename, bool addAffixPatternMappings);

    template <typename T1, typename T2>
    static inline int8_t getNumBitsFromValues(const ::std::map<T1, T2> &wordToData);

    ~LexicalDictionaryBuilder();

private:
    static void write(::std::ofstream& writer,
                      DictionaryLogger& logger,
                      const ::inflection::util::ULocale& locale,
                      int8_t bitsTypesSingletons,
                      int8_t bitsPropertyMapId,
                      int8_t bitsPropertyMapKeyId,
                      int8_t bitsPropertyMapValuesSize,
                      const inflection::dictionary::metadata::MarisaTrie<uint64_t>& wordsToDataTrie,
                      const std::vector<int64_t>& typeSingletons,
                      const inflection::dictionary::metadata::CompressedArray<uint64_t>& wordsToDataSingletons,
                      const inflection::dictionary::metadata::StringArrayContainer& propertyNameToKeyId,
                      const inflection::dictionary::metadata::StringContainer& propertyValuesStringContainer,
                      const inflection::dictionary::metadata::CompressedArray<int32_t>& propertyValueMaps,
                      const inflection::dictionary::metadata::StringArrayContainer& typesStringContainer,
                      bool hasInflectionTable);

    template <typename T>
    static inline void writeValue(uint64_t &valueBase, int32_t start, int32_t len, T valueToWrite);

    LexicalDictionaryBuilder() = delete;
    LexicalDictionaryBuilder(LexicalDictionaryBuilder&) = delete;
    LexicalDictionaryBuilder& operator=(const LexicalDictionaryBuilder&) = delete;
};

template <typename T1, typename T2>
inline int8_t LexicalDictionaryBuilder::getNumBitsFromValues(const ::std::map<T1, T2> &wordToData) {
    uint64_t maxData = 0;
    for (const auto& entry : wordToData) {
        auto value = (uint64_t) entry.second;
        if (value > maxData) {
            maxData = value;
        }
    }
    int16_t numBits = 0;
    while (maxData != 0) {
        numBits++;
        maxData >>= 1;
    }
    if (numBits == 0) {
        numBits = 1;
    }
    return numBits;
}
