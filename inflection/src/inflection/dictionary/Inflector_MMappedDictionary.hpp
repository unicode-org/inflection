/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/dictionary/metadata/StringContainer.hpp>
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <optional>

/*
 * This class is a memory-mapped-backed replacement for Inflector_InflectionDictionary
 */
class inflection::dictionary::Inflector_MMappedDictionary final {
public:
    // 7 bytes + null terminator
    static constexpr int64_t VERSION { 3 }; // Bump this version if the binary file format changes.
    static constexpr int16_t ENDIANNESS_MARKER = 1;
    static constexpr int16_t OPTIONS = {  }; // Space reserved for future options. Also used to align data structures after this header. Ideally align to 8 byte boundaries for 64-bit CPU architectures.
    static constexpr const char16_t * const INFLECTION_KEY = u"inflection";

public:
    explicit Inflector_MMappedDictionary(const Inflector_MMappedDictionary&) = delete;
    explicit Inflector_MMappedDictionary(Inflector_MMappedDictionary&&) = delete;
    Inflector_MMappedDictionary& operator=(const Inflector_MMappedDictionary&) = delete;

public:
    explicit Inflector_MMappedDictionary(inflection::util::MemoryMappedFile& memoryMappedFile, const ::std::u16string& sourcePath, const ::inflection::dictionary::DictionaryMetaData_MMappedDictionary &dictionary);
    ~Inflector_MMappedDictionary();


public:
    std::optional<int16_t> getInflectionPatternIndexFromName(std::u16string_view name) const;
    Inflector_InflectionPattern getInflectionPattern(int32_t index) const;
    std::vector<Inflector_InflectionPattern> getInflectionPatternsFromSuffix(std::u16string_view suffix) const;
    bool getInflectionPatternIdentifiers(std::vector<int32_t>& inflectionIdentifiers, std::u16string_view word) const;

private:
    const inflection::util::ULocale locale;

    int32_t grammemePatternsSize {  };
    const int64_t *grammemePatterns {  };

    // inflectionPatterns - originally a std::map<std::u16string, Inflector_InflectionPattern>
    inflection::dictionary::metadata::StringContainer inflectionSuffixes;
    // Reference from DictionaryMetaData
    const ::inflection::dictionary::DictionaryMetaData_MMappedDictionary &dictionary;
    inflection::dictionary::metadata::CompressedArray<int64_t> inflectionsArray;
    int8_t numBitsForGrammemesIdx {  };
    int8_t numBitsForSuffixIdx {  };
    int8_t numBitsForSuffixToIdentifierRunsIdx {  };
    int8_t numBitsForSuffixToIdentifierRunsLen {  };
    int32_t frequencyArraySize {  };
    const int32_t *frequenciesArray {  };
    inflection::dictionary::metadata::MarisaTrie<int32_t> identifierToInflectionPatternTrie;

    /** suffixPatterns - originally a std::map<std::u16string, ::std::map<Inflector_Suffix, ::std::u16string>> */
    inflection::dictionary::metadata::MarisaTrie<int32_t> suffixToIdentifierRunsTrie;
    inflection::dictionary::metadata::CompressedArray<int32_t> suffixesToInflectionsIDArray;

    int8_t numBitsForFrequencyIdx {  };
    int8_t numBitsForPOSIdx {  };
    int8_t numBitsForLemmaSuffixesLen {  };
    int8_t numBitsForNumOfInflections {  };
    int8_t numBitsForPrefixIdx {  }; // reserved
    bool hasSuffixToIdentifiersMapping {  };

    friend class DictionaryMetaData_MMappedDictionary;
    friend class Inflector_InflectionPattern;
    friend class Inflector_Inflection;
    friend class Inflector_Suffix;
};
