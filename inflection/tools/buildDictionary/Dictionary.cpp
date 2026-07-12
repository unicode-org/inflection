/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "Dictionary.hpp"
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/exception/FileNotFoundException.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <unicode/uchar.h>
#include <unicode/ustring.h>
#include <string>
#include <fstream>
#include <istream>
#include <set>

static constexpr char16_t COLON_SEPARATOR[] = { u": " };
static constexpr char16_t EQUALS_SEPARATOR = { u'=' };
static constexpr char16_t SPACE_SEPARATOR[] = { u" " };

static ::std::u16string_view trimWhitespace(::std::u16string_view str) {
    int32_t len = int32_t(str.length());
    int32_t start = 0;
    while (start < len && u_isWhitespace(str[start])) {
        start++;
    }
    int32_t end = len;
    while (end > start && u_isWhitespace(str[end - 1])) {
        end--;
    }
    return str.substr(start, end - start);
}

Dictionary::Dictionary(const ::inflection::util::ULocale& locale)
    : locale(locale)
{
}

Dictionary::~Dictionary()
{
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

const StringPool& Dictionary::getStringPool() const
{
    return stringSingletons;
}

void Dictionary::loadPartsOfSpeech(Dictionary* destination, const ::std::string& sourcePath)
{
    ::std::ifstream reader(sourcePath.c_str(), std::ios::in|std::ios::binary);
    if (!reader) {
        throw ::inflection::exception::FileNotFoundException(
                inflection::util::StringViewUtils::to_u16string(sourcePath) + u" file not found");
    }
    // Estimate entry count from file size to pre-size hash maps and reduce rehashing.
    reader.seekg(0, std::ios::end);
    auto fileSize = reader.tellg();
    reader.seekg(0, std::ios::beg);
    if (fileSize > 0) {
        auto estimatedEntries = size_t(fileSize) / APPROXIMATE_BYTES_PER_LINE;
        if (estimatedEntries > destination->stringSingletons.bucket_count()) {
            destination->stringSingletons.reserve(estimatedEntries);
        }
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
            throw ::inflection::exception::IllegalStateException(u"Too many properties requested for " + locale.toString() + u" type=" + std::u16string(type));
        }
        auto savedString(stringSingletons.intern(type));
        typeToValue.emplace(savedString, typeValue);
        valueToType.emplace(typeValue, savedString);
    }
    else {
        typeValue = existingTypeValue->second;
    }
    return typeValue;
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

        std::u16string_view lineView(trimWhitespace(line));
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
        auto phrase(stringSingletons.intern(lineView.substr(0, colonIdx)));
        if (phrase.empty()) {
            // Probably from a supplementary file at the end. Ignore it.
            continue;
        }
        auto metadata = ::inflection::util::StringViewUtils::trim(lineView.substr(colonIdx + 1));
        int64_t combinedType = 0;
        ::std::map<::std::u16string_view, ::std::vector<::std::u16string_view>> propertyValues;
        for (::inflection::util::DelimitedStringIterator typeIterator(metadata, SPACE_SEPARATOR); typeIterator.hasNext(); ++typeIterator) {
            auto type = *typeIterator;
            auto typeValueIdx = type.find(EQUALS_SEPARATOR);
            if (typeValueIdx != std::u16string::npos) {
                auto propertyName(stringSingletons.intern(type.substr(0, typeValueIdx)));
                auto propertyValue(stringSingletons.intern(type.substr(typeValueIdx + 1)));
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
        wordsToTypes[phrase] = combinedType;
        ::std::u16string normalizedValue;
        transform(&normalizedValue, phrase);
        if (normalizedValue != phrase) {
            // Don't override previous value because this isn't an exact value.
            auto normalizedKey(stringSingletons.intern(normalizedValue));
            wordsToTypes.emplace(normalizedKey, combinedType);
            if (!propertyValues.empty()) {
                wordToPropertyValue.emplace(normalizedKey, propertyValues);
            }
        }
        if (!propertyValues.empty()) {
            // replace value when necessary. Move after the normalized copy above.
            wordToPropertyValue[phrase] = std::move(propertyValues);
        }
    }
}
