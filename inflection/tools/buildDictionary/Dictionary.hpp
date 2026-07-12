/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/util/ULocale.hpp>
#include "StringPool.hpp"
#include <cstdint>
#include <map>
#include <vector>

class Dictionary
{
public:
    static Dictionary* setupDictionary(const ::inflection::util::ULocale& locale, const ::std::string& sourcePath, const ::std::string& additionalSourcePath);

    const ::inflection::util::ULocale& getLocale() const;
    const ::std::map<std::u16string_view, int64_t>& getTypeToValue() const;
    const ::std::map<std::u16string_view, int64_t>& getWordsToTypes() const;
    const ::std::map<int64_t, ::std::u16string_view>& getValueToType() const;
    const ::std::map<std::u16string_view, ::std::map<std::u16string_view, ::std::vector<std::u16string_view>>>& getWordToPropertyValue() const;
    const StringPool& getStringPool() const;

    int64_t createGrammemeTypeIfNonExistant(::std::u16string_view type);

private:
    ::inflection::util::ULocale locale;
    ::std::map<::std::u16string_view, int64_t> typeToValue {  };
    ::std::map<int64_t, ::std::u16string_view> valueToType {  };
    ::std::map<std::u16string_view, int64_t> wordsToTypes {  };
    ::std::map<::std::u16string_view, ::std::map<::std::u16string_view, ::std::vector<::std::u16string_view>>> wordToPropertyValue {  };
    StringPool stringSingletons;

    /**
     * Provides an estimated ratio of file size to number of entries to reduce rehashing of an unordered map.
     * Average ~50 bytes per line including the word + lowercase variant.
     */
    static constexpr uint32_t APPROXIMATE_BYTES_PER_LINE = 50;

    ::std::u16string* transform(::std::u16string* dest, ::std::u16string_view str);

    void extractPartsOfSpeech(::std::ifstream& in);
    static void loadPartsOfSpeech(Dictionary* destination, const ::std::string& sourcePath);

    Dictionary(const ::inflection::util::ULocale& locale);
public:
    ~Dictionary();
    Dictionary(const Dictionary&) = delete;
    Dictionary& operator=(const Dictionary&) = delete;
};


