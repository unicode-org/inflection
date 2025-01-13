/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include "DictionaryLogger.hpp"
#include "OverlappingSpanArray.hpp"
#include "inflection/util/MemoryMappedFile.hpp"
#include "inflection/dictionary/metadata/StringContainer.hpp"
#include "inflection/dictionary/metadata/MarisaTrie.hpp"

#include "Inflector_InflectionDictionary.hpp"
#include <libxml2/libxml/tree.h>

/*
 * This tool is used to build the memory mappable inflection data. It does the following steps:
 *
 * 1) Loads the xml data associated with the locale into the Inflector_* data structures in this folder (These are not the same as the ones in the library)
 * 2) Maps all the xml data into special data structures used to flatten out any maps and vectors that were read from the xml file
 * 3) Uses those special data structures to predict offsets for any containers that are nested (i.e. map<string, map<data, data>>)
 * 4) Writes the flattened data to a binary file
 *
 * The binary file looks like this:
 * |=============================================================================================|
 * | HEADER                                                                                      |
 * |---------------------------------------------------------------------------------------------|
 * | numPatterns                                                      - int16_t                  |
 * | grammemePatternsSize                                             - int16_t                  |
 * | grammemePatterns                                                 - StringArrayContainer     |
 * | inflection_Suffixes                                              - StringContainer          |
 * |                                                                                             |
 * | |---------------------------------- InflectionPatterns -----------------------------------| |
 * | | InflectionPattern[numPatterns] (see header for structure)      - struct[]               | |
 * | |                                                                                         | |
 * | | frequency (contains binary size of following 3 arrays)         - int32_t                | |
 * | | lemmaSuffixesLen (length of lemmaSuffixOffset)                 - int16_t                | |
 * | | numOfInflections (length of inflectionsIndex)                  - int16_t                | |
 * | | lemmaSuffixOffset[] (index to start in lemmaSuffixesOffsetsContainer) - int32_t[]       | |
 * | | inflectionsIndex[] (index to start in inflections)             - int32_t[]              | |
 * | |-----------------------------------------------------------------------------------------| |
 * |                                                                                             |
 * | inflections                                                      - CompressedArray          |
 * | numBitsForGrammemesIdx                                           - int8_t                   |
 * | numBitsForSuffixIdx                                              - int8_t                   |
 * | numBitsForSuffixToIdentifierRunsIdx                              - int8_t                   |
 * | numBitsForSuffixToIdentifierRunsLen                              - int8_t                   |
 * | lemmaSuffixesOffsetsContainerSize                                - int32_t                  |
 * | lemmaSuffixesOffsetsContainer                                    - int32_t[]                |
 * | identifierToInflectionPatternTrie                                - MarisaTrie               |
 * | suffixToIdentifierRunsTrie                                       - MarisaTrie               |
 * | suffixesToInflections                                            - CompressedArray          |
 * |==============================================================================================
 *
 * See InflectionDictionary.hpp for many of the structure definitions. Most of the int32_t offset pointers just point to "nested" structures.
 */
class InflectionDictionary {
public:
    explicit InflectionDictionary(const ::inflection::util::ULocale &locale, const Inflector_InflectionDictionary &dictionary, bool addAffixPatternMappings); // load from dictionary
    ~InflectionDictionary();
    InflectionDictionary(const InflectionDictionary&) = delete;
    InflectionDictionary& operator=(const InflectionDictionary&) = delete;

    int32_t getId(std::u16string_view identifierStr) const;
    void write(::std::ofstream& writer, DictionaryLogger& logger) const;

    static Inflector_InflectionDictionary* readXML(const ::inflection::util::ULocale& locale,
                                                   const std::string& source,
                                                   const std::map<::std::u16string_view, int32_t>& grammemeStringIndices);

private:
    static Inflector_InflectionDictionary* getInflectionPatterns(const ::inflection::util::ULocale& locale, xmlDocPtr xmlDoc,
                                                                 const std::map<::std::u16string_view, int32_t>& grammemeStringIndices);
    int32_t getGrammemeSingletonIndex(std::map<int64_t, int32_t>& grammemesSingletons, int64_t grammemes);

private:
    const inflection::util::ULocale locale;

    // inflectionPatterns - originally a std::map<std::u16string, Inflector_InflectionPattern>
    struct Inflection {
        int32_t suffixOffset {  };
        int32_t grammemesIdx {  };

        inline Inflection() {};
        ~Inflection();
    };

    struct InflectionPattern {
        int32_t frequency {  };
        int16_t lemmaSuffixesLen {  };
        int16_t numOfInflections {  };
        int32_t inflectionsIndex {  }; // Starting index into inflectionsArray

        inline InflectionPattern() {};
        ~InflectionPattern();
    };
    inflection::dictionary::metadata::StringContainer *inflection_Suffixes {  };
    inflection::dictionary::metadata::MarisaTrie<int32_t> *identifierToInflectionPatternTrie {  };
    std::vector<int64_t> grammemePatterns {  };
    std::vector<int64_t> inflectionsArray {  };
    std::vector<int32_t> frequencyArray {  };
    int8_t numBitsForGrammemesIdx {  };
    int8_t numBitsForSuffixIdx {  };
    int8_t numBitsForSuffixToIdentifierRunsIdx {  };
    int8_t numBitsForSuffixToIdentifierRunsLen {  };

    int8_t numBitsForFrequencyIdx {  };
    int8_t numBitsForPOSIdx {  };
    int8_t numBitsForLemmaSuffixesLen {  };
    int8_t numBitsForNumOfInflections {  };

    // suffixPatterns -     originally a std::map<std::u16string, ::std::map<Inflector_Suffix, ::std::u16string>>
    inflection::dictionary::metadata::MarisaTrie<int32_t> *suffixToIdentifierRunsTrie {  };
    OverlappingSpanArray<int64_t> suffixesToInflections {  };
};

