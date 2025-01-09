/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/util/ULocale.hpp>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

class Dictionary
{
public:
    static Dictionary* setupDictionary(const ::inflection::util::ULocale& locale, const ::std::string& sourcePath, const ::std::string& additionalSourcePath);

    const ::inflection::util::ULocale& getLocale() const;
    const ::std::map<std::u16string_view, int64_t>& getTypeToValue() const;
    const ::std::map<std::u16string_view, int64_t>& getWordsToTypes() const;
    const ::std::map<int64_t, ::std::u16string_view>& getValueToType() const;
    const ::std::map<int64_t, std::u16string_view>& getType() const;
    const ::std::map<std::u16string_view, ::std::map<std::u16string_view, ::std::vector<std::u16string_view>>>& getWordToPropertyValue() const;

    int64_t createGrammemeTypeIfNonExistant(::std::u16string_view type);
    std::u16string_view getStringSingleton(::std::u16string_view view);

private:
    ::inflection::util::ULocale locale;
    ::std::map<int64_t, ::std::u16string_view> types {  };
    ::std::map<::std::u16string_view, int64_t> typeToValue {  };
    ::std::map<int64_t, ::std::u16string_view> valueToType {  };
    ::std::map<std::u16string_view, int64_t> wordsToTypes {  };
    ::std::map<::std::u16string_view, ::std::map<::std::u16string_view, ::std::vector<::std::u16string_view>>> wordToPropertyValue {  };
    ::std::unordered_map<::std::u16string_view, char16_t*> stringSingletons;

    /**
     * The first prime below an order of magnitude of 2^n that is a reasonable default starting size for a hash map.
     * Right now we have 128K-1.3M entries.
     */
    static constexpr uint32_t DEFAULT_SINGLETON_MAP_SIZE = 262139;

    ::std::u16string* transform(::std::u16string* dest, ::std::u16string_view str);

    void extractPartsOfSpeech(::std::ifstream& in);
    static void loadPartsOfSpeech(Dictionary* destination, const ::std::string& sourcePath);

    Dictionary(const ::inflection::util::ULocale& locale);
public:
    ~Dictionary();
};


