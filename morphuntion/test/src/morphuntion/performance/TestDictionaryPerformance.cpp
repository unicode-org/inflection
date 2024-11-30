/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include "PerformanceUtils.h"

#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>
#include <marisa/iostream.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

constexpr int32_t DEFAULT_MAXMIMUM_WORDS_TO_TEST = 250000;

int64_t DictionaryPerformanceInitialize(const morphuntion::util::ULocale& locale)
{
    auto initStart = std::chrono::high_resolution_clock::now();
    ::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale);
    auto initEnd = std::chrono::high_resolution_clock::now();

    return (int64_t)(std::chrono::duration<double, std::milli>(initEnd - initStart).count());
}

int64_t DictionaryPerformanceGetCombinedBinaryType(const morphuntion::util::ULocale& locale, const std::vector<::std::u16string>& words)
{
    auto dictionary = ::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale);
    int64_t type = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& word : words) {
        dictionary->getCombinedBinaryType(&type, word);
    }
    auto end = std::chrono::high_resolution_clock::now();

    return (int64_t)(std::chrono::duration<double, std::milli>(end - start).count());
}

int64_t DictionaryPerformanceGetPropertyValues(const morphuntion::util::ULocale& locale, const std::vector<::std::u16string>& words)
{
    ::std::map<::std::string, ::std::u16string, std::less<>> importantKeys({
          {"ar", u"inflection"},
          {"bg", u"inflection"},
          {"ca", u"inflection"},
          {"cs", u"inflection"},
          {"da", u"inflection"},
          {"de", u"inflection"},
          {"en", u"inflection"},
          {"de", u"inflection"},
          {"es", u"inflection"},
          {"fi", u"inflection"},
          {"fr", u"inflection"},
          {"ja", u"synonym"},
          {"he", u"inflection"},
          {"hi", u"inflection"},
          {"hr", u"inflection"},
          {"hu", u"inflection"},
          {"is", u"inflection"},
          {"it", u"inflection"},
          {"kk", u"inflection"},
          {"nb", u"inflection"},
          {"nl", u"inflection"},
          {"pl", u"inflection"},
          {"pt", u"inflection"},
          {"ro", u"inflection"},
          {"ru", u"inflection"},
          {"sk", u"inflection"},
          {"sv", u"inflection"},
          {"tr", u"inflection"},
          {"uk", u"inflection"},
          {"yue", u"synonym"},
          {"zh", u"synonym"}
    });

    auto dictionary = ::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale);

    auto importantKeysValue = importantKeys.find(locale.getLanguage());
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point end = start;
    if (importantKeysValue != importantKeys.end()) {
        auto keyToCheck = importantKeysValue->second;
        start = std::chrono::high_resolution_clock::now();
        for (const auto& word : words) {
            dictionary->getPropertyValues(word, keyToCheck);
        }
        end = std::chrono::high_resolution_clock::now();
    }

    return (int64_t)(std::chrono::duration<double, std::milli>(end - start).count());
}

TEST_CASE("TestDictionaryPerformance#testAllLanguages", "[.]")
{
    ::std::set<::morphuntion::util::ULocale, ::std::greater<>> locales;
    auto ascendingLocales(::morphuntion::util::LocaleUtils::getSupportedLocaleList());
    locales.insert(ascendingLocales.begin(), ascendingLocales.end());
    
    auto delimiter = ",";

    PerfTable<std::ofstream> csvTable("testDictionaryPerformance.csv");
    csvTable.writeRow([delimiter](std::ofstream& writer)
    {
        writer  << "locale"
                << delimiter
                << "init ms"
                << delimiter
                << "getCombinedBinaryType ms"
                << delimiter
                << "getPropertyValues ms"
                << delimiter
                << "words"
                << std::endl;
    });
    ::std::vector<::std::u16string> words;
    words.reserve(DEFAULT_MAXMIMUM_WORDS_TO_TEST);

    for (const auto& locale : locales) {
        words.clear();
        int64_t initTime = DictionaryPerformanceInitialize(locale);
        auto dictionary = ::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale);

        int32_t wordCount = 0;
        while (wordCount < DEFAULT_MAXMIMUM_WORDS_TO_TEST) {
            for (const auto& word : npc(dictionary)->getKnownWords()) {
                words.emplace_back(word);
                if (++wordCount >= DEFAULT_MAXMIMUM_WORDS_TO_TEST) {
                    // That's enough to try out.
                    break;
                }
            }
            if (wordCount == 0) {
                // Nothing to test
                break;
            }
        }

        int64_t getCombinedBinaryTypeTime = DictionaryPerformanceGetCombinedBinaryType(locale, words);
        int64_t getPropertyValuesTime = DictionaryPerformanceGetPropertyValues(locale, words);

        csvTable.writeRow([locale, initTime, getCombinedBinaryTypeTime, getPropertyValuesTime, delimiter, wordCount](std::ofstream& writer)
        {
            writer  << locale.getName()
                    << delimiter
                    << initTime
                    << delimiter
                    << getCombinedBinaryTypeTime
                    << delimiter
                    << getPropertyValuesTime
                    << delimiter
                    << wordCount
                    << std::endl;
        });
    }
}
