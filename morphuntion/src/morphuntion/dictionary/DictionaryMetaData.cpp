/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dictionary/DictionaryMetaData.hpp>

#include <morphuntion/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>
#include <morphuntion/exception/IOException.hpp>
#include <morphuntion/exception/IllegalArgumentException.hpp>
#include <morphuntion/util/AutoFileDescriptor.hpp>
#include <morphuntion/util/ResourceLocator.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/LoggerConfig.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/npc.hpp>
#include <mutex>

namespace morphuntion::dictionary {

static std::mutex& CLASS_MUTEX() {
    static auto classMutex = new std::mutex();
    return *npc(classMutex);
}

DictionaryMetaData::DictionaryMetaData(DictionaryMetaData_MMappedDictionary* dictionary)
    : super()
    , dictionary(dictionary)
{
}

DictionaryMetaData::~DictionaryMetaData()
{
    delete dictionary;
}

static constexpr char16_t FILE_EXTENSION[] = { u".sdict" };
static constexpr char16_t BASE_PATH[] = { u"/dictionary/" };
static constexpr char16_t BASE_NAME[] = { u"mmappable" };

static ::std::u16string getResourcePath(const ::morphuntion::util::ULocale& locale) {
    auto localeLookup = (locale == morphuntion::util::LocaleUtils::ARABIC()) ? morphuntion::util::LocaleUtils::ROOT() : locale;
    const auto morphunRoot = ::morphuntion::util::ResourceLocator::getRootForLocale(localeLookup);
    auto dictionaryPath = morphunRoot + ::std::u16string(BASE_PATH) + ::std::u16string(BASE_NAME)
                + ::std::u16string(u"_")
                + ::morphuntion::util::StringViewUtils::to_u16string(locale.getLanguage())
                + ::std::u16string(FILE_EXTENSION);
    return dictionaryPath;
}

static ::morphuntion::util::ULocale getLanguageWithFallback(const ::morphuntion::util::ULocale& locale) {
    ::morphuntion::util::ULocale genericLanguage(locale.getLanguage());
    auto localePair = ::morphuntion::util::LocaleUtils::getFallbackPair(locale);
    // This map isn't needed long term. After it's looked up once, the caching in createDictionary will
    // prevent it from being used again for a given language.
    std::map<::morphuntion::util::ULocale, ::morphuntion::util::ULocale> fallbackMap({
        {::morphuntion::util::LocaleUtils::CANTONESE(), ::morphuntion::util::LocaleUtils::CHINESE()},
        {::morphuntion::util::LocaleUtils::SHANGHAINESE(), ::morphuntion::util::LocaleUtils::CHINESE()},
    });
    auto fallbackResult = fallbackMap.find(localePair.first);
    if (fallbackResult != fallbackMap.end()) {
        return fallbackResult->second;
    }
    return localePair.first;
}

static void logEmptyDictionaryWarning(const ::morphuntion::util::ULocale &locale, const ::morphuntion::exception::IOException& e) {
    if (::morphuntion::util::LoggerConfig::isWarnEnabled()) {
        ::morphuntion::util::Logger::warn(u"The dictionary for " + locale.toString()
            + u" could not be created. An empty dictionary will be used instead: " + e.getMessage());
    }
}

static DictionaryMetaData_MMappedDictionary* createDictionaryForLocale(const ::morphuntion::util::ULocale& locale) {
    ::morphuntion::util::ULocale genericLanguage(locale.getLanguage());
    try {
        auto path(getResourcePath(genericLanguage));
        if (!morphuntion::util::AutoFileDescriptor::isAccessibleFile(path)) {
            // This rarely happens, unless a fallback is needed. Calculate the fallback.
            path = getResourcePath(getLanguageWithFallback(locale));
        }
        return DictionaryMetaData_MMappedDictionary::createDictionary(path);
    } catch (const ::morphuntion::exception::IOException& e) {
        logEmptyDictionaryWarning(locale, e);
        return new DictionaryMetaData_MMappedDictionary(locale);
    }
}

static ::std::map<::std::string, DictionaryMetaData*, std::greater<>>* DICTIONARY_CACHE()
{
    static auto DICTIONARY_CACHE_ = new ::std::map<::std::string, DictionaryMetaData*, std::greater<>>();
    return DICTIONARY_CACHE_;
}

const DictionaryMetaData* DictionaryMetaData::createDictionary(const ::morphuntion::util::ULocale& locale)
{
    DictionaryMetaData* result;
    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    auto dictionaryCache = DICTIONARY_CACHE();
    auto existingDictionary = dictionaryCache->find(locale.getLanguage());
    if (existingDictionary != dictionaryCache->end()) {
        result = existingDictionary->second;
    }
    else {
        result = new DictionaryMetaData(createDictionaryForLocale(locale));
        dictionaryCache->emplace(locale.getLanguage(), result);
    }
    return result;
}

::std::u16string* DictionaryMetaData::transform(::std::u16string* dest, std::u16string_view str, const ::morphuntion::util::ULocale& locale)
{
    return ::morphuntion::util::StringViewUtils::lowercase(dest, str, locale);
}

int64_t* DictionaryMetaData::getCombinedBinaryType(int64_t* result, std::u16string_view word) const
{
    *npc(result) = 0;
    auto combinedType = npc(dictionary)->getWordType(word);
    if (!combinedType) {
        ::std::u16string normalized;
        transform(&normalized, word, npc(dictionary)->getLocale());
        if (normalized != word) {
            combinedType = npc(dictionary)->getWordType(normalized);
        }
    }
    if (!combinedType) {
        return nullptr;
    }
    *npc(result) = *combinedType;
    return result;
}

::std::vector<::std::u16string> DictionaryMetaData::getPropertyNames(int64_t binaryProperties) const
{
    return npc(dictionary)->getTypesOfValues(binaryProperties);
}

::std::u16string DictionaryMetaData::getPropertyName(int64_t singleProperty) const
{
    return npc(dictionary)->getTypeOfValue(singleProperty).value_or(std::u16string());
}

bool DictionaryMetaData::isKnownWord(std::u16string_view word) const
{
    int64_t combinedType = 0;
    return getCombinedBinaryType(&combinedType, word) != nullptr;
}

bool DictionaryMetaData::hasProperty(std::u16string_view word, std::u16string_view partOfSpeech) const
{
    auto property = npc(dictionary)->getValueOfType(partOfSpeech);
    if (!property) {
        return false;
    }
    return hasAllProperties(word, *property);
}

bool DictionaryMetaData::hasAllProperties(std::u16string_view word, int64_t partOfSpeech) const
{
    int64_t combinedType = 0;
    return getCombinedBinaryType(&combinedType, word) != nullptr && ((combinedType & partOfSpeech) == partOfSpeech);
}

bool DictionaryMetaData::hasAnyProperty(std::u16string_view word, int64_t partOfSpeech) const
{
    int64_t combinedType = 0;
    return getCombinedBinaryType(&combinedType, word) != nullptr && ((combinedType & partOfSpeech) != 0);
}

int64_t DictionaryMetaData::getBinaryProperties(const ::std::vector<::std::u16string>& properties) const
{
    int64_t combinedType = 0;
    if (properties.empty()) {
        return combinedType;
    }
    if (getBinaryProperties(&combinedType, properties) == nullptr) {
        throw ::morphuntion::exception::IllegalArgumentException(u"Properties are not recognized");
    }
    return combinedType;
}

int64_t* DictionaryMetaData::getBinaryProperties(int64_t* result, const ::std::vector<::std::u16string>& properties) const
{
    *npc(result) = npc(dictionary)->getValuesOfTypes(properties);
    if (*npc(result) == 0) {
        return nullptr;
    }
    return result;
}

::std::vector<::std::u16string> DictionaryMetaData::getProperties(std::u16string_view word) const
{
    int64_t combinedType = 0;
    if (getCombinedBinaryType(&combinedType, word) != nullptr) {
        return getPropertyNames(combinedType);
    }
    return {};
}

::std::vector<::std::u16string> DictionaryMetaData::getPropertyValues(std::u16string_view word, std::u16string_view partOfSpeech) const
{
    ::std::vector<::std::u16string> result;
    getPropertyValues(&result, word, partOfSpeech);
    return result;
}

bool DictionaryMetaData::getPropertyValues(::std::vector<::std::u16string>* result, std::u16string_view word, std::u16string_view partOfSpeech) const
{
    if (result != nullptr) {
        result->clear();
    }
    auto exists = npc(dictionary)->getWordPropertyValues(result, word, partOfSpeech);
    if (!exists && !morphuntion::util::StringViewUtils::isAllLowerCase(word) && !npc(dictionary)->getWordType(word)) {
        ::std::u16string normalized;
        transform(&normalized, word, npc(dictionary)->getLocale());
        if (normalized != word) {
            exists = npc(dictionary)->getWordPropertyValues(result, normalized, partOfSpeech);
        }
    }
    return exists;
}

DictionaryKeyIterator DictionaryMetaData::getKnownWords() const
{
    return npc(dictionary)->getAllWords();
}

int32_t DictionaryMetaData::getKnownWordsSize() const
{
    return npc(dictionary)->getAllWordsSize();
}

} // namespace morphuntion::dictionary
