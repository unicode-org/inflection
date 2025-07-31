/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "Dictionary.hpp"
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/exception/FileNotFoundException.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <unicode/ustring.h>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <set>

static constexpr char16_t COLON_SEPARATOR[] = { u": " };
static constexpr char16_t EQUALS_SEPARATOR = { u'=' };
static constexpr char16_t SPACE_SEPARATOR[] = { u" " };

Dictionary::Dictionary(const ::inflection::util::ULocale& locale)
    : locale(locale)
{
    stringSingletons.reserve(DEFAULT_SINGLETON_MAP_SIZE);
    stringSingletons.max_load_factor(0.75f);
}

Dictionary::~Dictionary()
{
    for (const auto& entry : stringSingletons) {
        delete []entry.second;
    }
}

Dictionary* Dictionary::setupDictionary(const ::inflection::util::ULocale& locale, const ::std::string& sourcePath, const ::std::string& additionalSourcePath)
{
    ::inflection::util::ULocale genericLanguage(locale.getLanguage());
    auto dictionary = new Dictionary(genericLanguage);
    loadPartsOfSpeech(dictionary, sourcePath);
    if (!additionalSourcePath.empty()) {
        loadPartsOfSpeech(dictionary, additionalSourcePath);
    }
    return dictionary;
}

const ::inflection::util::ULocale& Dictionary::getLocale() const
{
    return locale;
}

const ::std::map<int64_t, ::std::u16string_view>& Dictionary::getType() const
{
    return types;
}

const ::std::map<::std::u16string_view, int64_t>& Dictionary::getTypeToValue() const
{
    return typeToValue;
}

const ::std::map<int64_t, ::std::u16string_view>& Dictionary::getValueToType() const
{
    return valueToType;
}

const ::std::map<::std::u16string_view, int64_t>& Dictionary::getWordsToTypes() const
{
    return wordsToTypes;
}

const ::std::map<::std::u16string_view, ::std::map<::std::u16string_view, ::std::vector<::std::u16string_view>>>& Dictionary::getWordToPropertyValue() const
{
    return wordToPropertyValue;
}

void Dictionary::loadPartsOfSpeech(Dictionary* destination, const ::std::string& sourcePath)
{
    ::std::ifstream reader(sourcePath.c_str(), std::ios::in|std::ios::binary);
    if (!reader) {
        throw ::inflection::exception::FileNotFoundException(
                inflection::util::StringViewUtils::to_u16string(sourcePath) + u" file not found");
    }
    destination->extractPartsOfSpeech(reader);
}

::std::u16string* Dictionary::transform(::std::u16string* dest, ::std::u16string_view str)
{
    return ::inflection::util::StringViewUtils::lowercase(dest, str, locale);
}

int64_t Dictionary::createGrammemeTypeIfNonExistant(::std::u16string_view type)
{
    int64_t typeValue;
    auto existingTypeValue = typeToValue.find(type);
    if (existingTypeValue == typeToValue.end()) {
        typeValue = int64_t(1LL) << typeToValue.size();
        if (typeValue <= 0) {
            std::cerr << "Existing grammemes:" << std::endl;
            for (const auto& [key, value] : typeToValue) {
                std::cerr << inflection::util::StringViewUtils::to_string(key) << std::endl;
            }
            throw ::inflection::exception::IllegalStateException(u"Too many properties requested for " + locale.toString() + u" type=" + std::u16string(type));
        }
        auto savedString(getStringSingleton(type));
        typeToValue.emplace(savedString, typeValue);
        valueToType.emplace(typeValue, savedString);
        types.emplace(typeValue, savedString);
    }
    else {
        typeValue = existingTypeValue->second;
    }
    return typeValue;
}

char16_t *strdup(std::u16string_view str) {
    auto len = int32_t(str.length() + 1);
    auto result = new char16_t[len];
    u_memcpy((UChar *)result, (const UChar *)str.data(), len);
    return result;
}

std::u16string_view Dictionary::getStringSingleton(::std::u16string_view view)
{
    auto existingEntry = stringSingletons.find(view);
    if (existingEntry == stringSingletons.end()) {
        auto rawChars = strdup(view);
        ::std::u16string_view key(rawChars, view.length());
        stringSingletons.emplace(key, rawChars);
        return key;
    }
    return existingEntry->first;
}

static constexpr int32_t DEFAULT_BUFFER_SIZE = 512;

void
Dictionary::extractPartsOfSpeech(::std::ifstream& in)
{
    bool checkFirstLine = true;
    ::std::u16string endline(u"====");
    ::std::string cline;
    ::std::u16string line;
    cline.reserve(DEFAULT_BUFFER_SIZE);
    line.reserve(DEFAULT_BUFFER_SIZE);
    while (in) {
        cline.clear();
        line.clear();
        ::std::getline(in, cline);
        ::inflection::util::StringViewUtils::convert(&line, cline);

        std::u16string_view lineView(line);
        if (checkFirstLine) {
            checkFirstLine = false;
            if (lineView.starts_with(u"version https://git-lfs.github.com/")) {
                throw ::inflection::exception::InvalidConfigurationException(u"Malformed file for language " + locale.toString() + u". Did you forget to use git LFS?");
            }
        }
        if (lineView.starts_with(endline)) {
            break;
        }
        auto colonIdx = lineView.find(COLON_SEPARATOR);
        auto phrase(getStringSingleton(lineView.substr(0, colonIdx)));
        auto metadata = ::inflection::util::StringViewUtils::trim(lineView.substr(colonIdx + 1));
        int64_t combinedType = 0;
        ::std::map<::std::u16string_view, ::std::vector<::std::u16string_view>> propertyValues;
        for (::inflection::util::DelimitedStringIterator typeIterator(metadata, SPACE_SEPARATOR); typeIterator.hasNext(); ++typeIterator) {
            auto type = *typeIterator;
            auto typeValueIdx = type.find(EQUALS_SEPARATOR);
            if (typeValueIdx != std::u16string::npos) {
                auto propertyName(getStringSingleton(type.substr(0, typeValueIdx)));
                auto propertyValue(getStringSingleton(type.substr(typeValueIdx + 1)));
                if (propertyValues.empty()) {
                    propertyValues.emplace(propertyName, ::std::vector<::std::u16string_view>({propertyValue}));
                } else {
                    auto existingValues = propertyValues.find(propertyName);
                    if (existingValues == propertyValues.end()) {
                        propertyValues.emplace(propertyName, ::std::vector<::std::u16string_view>({propertyValue}));
                    } else {
                        existingValues->second.push_back(propertyValue);
                    }
                }
            } else {
                combinedType |= createGrammemeTypeIfNonExistant(type);
            }
        }
        for (const auto &itr : propertyValues) {
            if (itr.second.size() > UINT8_MAX) {
                throw ::inflection::exception::IllegalStateException(u"Too many property values of property=" + ::std::u16string(itr.first) + u" for the phrase " + std::u16string(phrase) + u" in the locale " + locale.toString());
            }
        }
        if (!propertyValues.empty()) {
            // replace value when necessary.
            wordToPropertyValue[phrase] = propertyValues;
        }
        wordsToTypes[phrase] = combinedType;
        ::std::u16string normalizedValue;
        transform(&normalizedValue, phrase);
        if (normalizedValue != phrase) {
            // Don't override previous value because this isn't an exact value.
            auto normalizedKey(getStringSingleton(normalizedValue));
            wordsToTypes.emplace(normalizedKey, combinedType);
            if (!propertyValues.empty()) {
                wordToPropertyValue.emplace(normalizedKey, propertyValues);
            }
        }
    }
}
