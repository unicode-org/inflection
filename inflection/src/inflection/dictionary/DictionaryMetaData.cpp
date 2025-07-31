/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/DictionaryMetaData.hpp>

#include <inflection/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/util/AutoFileDescriptor.hpp>
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <mutex>

namespace inflection::dictionary {

static std::mutex& CLASS_MUTEX() {
    static auto classMutex = new std::mutex();
    return *npc(classMutex);
}

DictionaryMetaData::DictionaryMetaData(DictionaryMetaData_MMappedDictionary* dictionary)
    : super()
    , dictionary(npc(dictionary))
{
}

DictionaryMetaData::~DictionaryMetaData()
{
}

static constexpr char16_t FILE_EXTENSION[] = { u".sdict" };
static constexpr char16_t BASE_PATH[] = { u"/dictionary/" };
static constexpr char16_t BASE_NAME[] = { u"mmappable" };

static ::std::u16string getResourcePath(const ::inflection::util::ULocale& locale) {
    const auto inflectionRoot = ::inflection::util::ResourceLocator::getRootForLocale(locale);
    auto dictionaryPath = inflectionRoot + ::std::u16string(BASE_PATH) + ::std::u16string(BASE_NAME)
                + ::std::u16string(u"_")
                + ::inflection::util::StringViewUtils::to_u16string(locale.getLanguage())
                + ::std::u16string(FILE_EXTENSION);
    return dictionaryPath;
}

static ::inflection::util::ULocale getLanguageWithFallback(const ::inflection::util::ULocale& locale) {
    auto localePair = ::inflection::util::LocaleUtils::getFallbackPair(locale);
    // This map isn't needed long term. After it's looked up once, the caching in createDictionary will
    // prevent it from being used again for a given language.
    std::map<::inflection::util::ULocale, ::inflection::util::ULocale> fallbackMap({
        {::inflection::util::LocaleUtils::CANTONESE(), ::inflection::util::LocaleUtils::CHINESE()},
        {::inflection::util::LocaleUtils::SHANGHAINESE(), ::inflection::util::LocaleUtils::CHINESE()},
    });
    auto fallbackResult = fallbackMap.find(localePair.first);
    if (fallbackResult != fallbackMap.end()) {
        return fallbackResult->second;
    }
    return localePair.first;
}

static void logEmptyDictionaryWarning(const ::inflection::util::ULocale &locale, const ::inflection::exception::IOException& e) {
    if (::inflection::util::LoggerConfig::isWarnEnabled()) {
        ::inflection::util::Logger::warn(u"The dictionary for " + locale.toString()
            + u" could not be created. An empty dictionary will be used instead: " + e.getMessage());
    }
}

static DictionaryMetaData_MMappedDictionary* createDictionaryForLocale(const ::inflection::util::ULocale& locale) {
    ::inflection::util::ULocale genericLanguage(locale.getLanguage());
    try {
        auto path(getResourcePath(genericLanguage));
        if (!inflection::util::AutoFileDescriptor::isAccessibleFile(path)) {
            // This rarely happens, unless a fallback is needed. Calculate the fallback.
            path = getResourcePath(getLanguageWithFallback(locale));
        }
        return DictionaryMetaData_MMappedDictionary::createDictionary(path);
    } catch (const ::inflection::exception::IOException& e) {
        logEmptyDictionaryWarning(locale, e);
        return new DictionaryMetaData_MMappedDictionary(locale);
    }
}

static ::std::map<::std::string, DictionaryMetaData*, std::less<>>* DICTIONARY_CACHE()
{
    static auto DICTIONARY_CACHE_ = new ::std::map<::std::string, DictionaryMetaData*, std::less<>>();
    return DICTIONARY_CACHE_;
}

const DictionaryMetaData* DictionaryMetaData::createDictionary(const ::inflection::util::ULocale& locale)
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

::std::u16string* DictionaryMetaData::transform(::std::u16string* dest, std::u16string_view str, const ::inflection::util::ULocale& locale)
{
    return ::inflection::util::StringViewUtils::lowercase(dest, str, locale);
}

int64_t* DictionaryMetaData::getCombinedBinaryType(int64_t* result, std::u16string_view word) const
{
    *npc(result) = 0;
    auto combinedType = dictionary->getWordType(word);
    if (!combinedType) {
        ::std::u16string normalized;
        transform(&normalized, word, dictionary->getLocale());
        if (normalized != word) {
            combinedType = dictionary->getWordType(normalized);
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
    return dictionary->getTypesOfValues(binaryProperties);
}

::std::u16string DictionaryMetaData::getPropertyName(int64_t singleProperty) const
{
    return dictionary->getTypeOfValue(singleProperty).value_or(std::u16string());
}

bool DictionaryMetaData::isKnownWord(std::u16string_view word) const
{
    int64_t combinedType = 0;
    return getCombinedBinaryType(&combinedType, word) != nullptr;
}

bool DictionaryMetaData::hasProperty(std::u16string_view word, std::u16string_view partOfSpeech) const
{
    auto property = dictionary->getValueOfType(partOfSpeech);
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
        throw ::inflection::exception::IllegalArgumentException(u"Properties are not recognized");
    }
    return combinedType;
}

int64_t* DictionaryMetaData::getBinaryProperties(int64_t* result, const ::std::vector<::std::u16string>& properties) const
{
    *npc(result) = dictionary->getValuesOfTypes(properties);
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
    auto exists = dictionary->getWordPropertyValues(result, word, partOfSpeech);
    if (!exists && !inflection::util::StringViewUtils::isAllLowerCase(word) && !dictionary->getWordType(word)) {
        ::std::u16string normalized;
        transform(&normalized, word, dictionary->getLocale());
        if (normalized != word) {
            exists = dictionary->getWordPropertyValues(result, normalized, partOfSpeech);
        }
    }
    return exists;
}

DictionaryKeyIterator DictionaryMetaData::getKnownWords() const
{
    return dictionary->getAllWords();
}

int32_t DictionaryMetaData::getKnownWordsSize() const
{
    return dictionary->getAllWordsSize();
}

} // namespace inflection::dictionary
