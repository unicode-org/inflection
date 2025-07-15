/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/metadata/fwd.hpp>
#include <inflection/dictionary/metadata/CompressedArray.hpp>
#include <inflection/dictionary/metadata/MarisaTrie.hpp>
#include <inflection/dictionary/metadata/StringContainer.hpp>
#include <inflection/dictionary/metadata/StringArrayContainer.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <optional>
#include <string_view>
#include <vector>

class INFLECTION_INTERNAL_API inflection::dictionary::DictionaryMetaData_MMappedDictionary
        : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

    static DictionaryMetaData_MMappedDictionary* createDictionary(const ::std::u16string& sourcePath);
    const ::inflection::util::ULocale& getLocale() const;
    ::std::optional<int64_t> getWordType(std::u16string_view word) const;
    ::std::optional<int64_t> getValueOfType(std::u16string_view type) const;
    int64_t getValuesOfTypes(const std::vector<std::u16string> &types) const;
    ::std::optional<::std::u16string> getTypeOfValue(int64_t value) const;
    ::std::vector<::std::u16string> getTypesOfValues(int64_t value) const;
private:
    void getPropertyMapInternalIdentifiers(std::vector<int32_t> &propertyIdentifiers, int32_t startingOffset, int32_t length) const;
    bool getWordPropertyInternalIdentifiers(std::vector<int32_t> &propertyIdentifiers, std::u16string_view word, int32_t propertyNameIdentifier) const;
public:
    bool getWordPropertyValues(::std::vector<::std::u16string>* result, std::u16string_view word, std::u16string_view property) const;
    ::inflection::dictionary::DictionaryKeyIterator getAllWords() const;
    int32_t getAllWordsSize() const;
    explicit DictionaryMetaData_MMappedDictionary(::inflection::util::MemoryMappedFile* memoryMappedRegion, const ::std::u16string& sourcePath);
    explicit DictionaryMetaData_MMappedDictionary(const inflection::util::ULocale& locale);
    ~DictionaryMetaData_MMappedDictionary() override;

private:
    DictionaryMetaData_MMappedDictionary(DictionaryMetaData_MMappedDictionary&) = delete;
    DictionaryMetaData_MMappedDictionary& operator=(const DictionaryMetaData_MMappedDictionary&) = delete;

public:
    enum class OptionBits {
        HAS_INFLECTION_TABLE = 1,
    };
    static constexpr int16_t OPTIONS = {  }; // Space reserved for options. Also used to align data structures after this header. Ideally align to 8 byte boundaries for 64-bit CPU architectures.
    static constexpr int16_t ENDIANNESS_MARKER = 1;
    static constexpr int8_t MAX_LANGUAGE_CODE_LENGTH = 4;
    static constexpr char MAGIC_MARKER[8] { "MORPHSD" };
    static constexpr int64_t VERSION { 7 }; // Bump this version if the binary file format changes.

private:
    int16_t options {  };
    ::inflection::util::ULocale locale;
    ::inflection::dictionary::metadata::StringArrayContainer typesStringContainer {  };
    int8_t bitsTypesSingletons {  };
    int8_t bitsPropertyMapId {  };
    int8_t bitsPropertyValueMapKey {  };
    int8_t bitsPropertyValueMapValuesSize {  };
    int32_t wordsToTypesSingletonsSize {  };
    ::inflection::dictionary::metadata::MarisaTrie<uint64_t> wordsToDataTrie {::std::map<std::u16string_view, uint64_t>()};
    const int64_t* const wordsToTypesSingletons {  };
    ::inflection::dictionary::metadata::CompressedArray<uint64_t> wordsToDataSingletons {::std::vector<uint64_t>()};
    ::inflection::dictionary::metadata::StringArrayContainer propertyNameToKeyId {  };
    ::inflection::dictionary::metadata::StringContainer propertyValuesStringContainer {  };
    ::inflection::dictionary::metadata::CompressedArray<int32_t> propertyValueMaps {::std::vector<int32_t>()};
    ::std::unique_ptr<::inflection::dictionary::Inflector> inflector {  };
    ::std::unique_ptr<::inflection::util::MemoryMappedFile> memoryMappedRegion {  };
    int32_t inflectionKeyIdentifier { -1 };
    int32_t bitsPropertyValueMapKeyMask {  };
    int32_t bitsPropertyValueMapKeySize {  };
    bool isDoubleStageLookup {  };

private:
    friend class Inflector;
    friend class Inflector_MMappedDictionary;
};
