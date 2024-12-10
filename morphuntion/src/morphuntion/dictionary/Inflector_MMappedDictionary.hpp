/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/MemoryMappedFile.hpp>
#include <morphuntion/dictionary/metadata/CompressedArray.hpp>
#include <morphuntion/dictionary/metadata/StringContainer.hpp>
#include <morphuntion/dictionary/metadata/MarisaTrie.hpp>
#include <morphuntion/dictionary/Inflector_InflectionPattern.hpp>
#include <optional>

/*
 * This class is a memory-mapped-backed replacement for Inflector_InflectionDictionary
 */
class morphuntion::dictionary::Inflector_MMappedDictionary final {
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
    explicit Inflector_MMappedDictionary(morphuntion::util::MemoryMappedFile& memoryMappedFile, const ::std::u16string& sourcePath, const ::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary &dictionary);
    ~Inflector_MMappedDictionary();


public:
    std::optional<int16_t> getInflectionPatternIndexFromName(std::u16string_view name) const;
    Inflector_InflectionPattern getInflectionPattern(int32_t index) const;
    std::vector<Inflector_InflectionPattern> getInflectionPatternsFromSuffix(std::u16string_view suffix) const;
    bool getInflectionPatternIdentifiers(std::vector<int32_t>& inflectionIdentifiers, std::u16string_view word) const;

private:
    const morphuntion::util::ULocale locale;

    int32_t grammemePatternsSize {  };
    const int64_t *grammemePatterns {  };

    // inflectionPatterns - originally a std::map<std::u16string, Inflector_InflectionPattern>
    morphuntion::dictionary::metadata::StringContainer inflection_Suffixes;
    // Reference from DictionaryMetaData
    const ::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary &dictionary;
    morphuntion::dictionary::metadata::CompressedArray<int64_t> inflectionsArray;
    int8_t numBitsForGrammemesIdx {  };
    int8_t numBitsForSuffixIdx {  };
    int8_t numBitsForSuffixToIdentifierRunsIdx {  };
    int8_t numBitsForSuffixToIdentifierRunsLen {  };
    int32_t frequencyArraySize {  };
    const int32_t *frequenciesArray {  };
    morphuntion::dictionary::metadata::MarisaTrie<int32_t> identifierToInflectionPatternTrie;

    /** suffixPatterns - originally a std::map<std::u16string, ::std::map<Inflector_Suffix, ::std::u16string>> */
    morphuntion::dictionary::metadata::MarisaTrie<int32_t> suffixToIdentifierRunsTrie;
    morphuntion::dictionary::metadata::CompressedArray<int32_t> suffixesToInflectionsIDArray;

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
