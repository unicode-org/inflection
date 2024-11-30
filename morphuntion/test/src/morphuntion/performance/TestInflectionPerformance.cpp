/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include "PerformanceUtils.h"

#include <morphuntion/dialog/InflectableStringConcept.hpp>
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticConcept.hpp>
#include <morphuntion/dialog/SemanticValue.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

constexpr int32_t DEFAULT_MAXIMUM_WORDS_TO_TEST = 100000;

struct InflectionInitData
{
    morphuntion::util::ULocale locale = morphuntion::util::ULocale("en_US");
    std::vector<std::u16string> nouns = { };
    int64_t words = 0;
    const morphuntion::dialog::SemanticFeatureModel* model = nullptr;
};

InflectionInitData* InflectionPerformanceInitialize(const morphuntion::util::ULocale& locale, int32_t MAXIMUM_WORDS_TO_TEST, int64_t& out_initTime, int64_t& out_initHeap)
{
    InflectionInitData* data = new InflectionInitData();
    out_initTime = 0;
    out_initHeap = 0;

    auto commonConceptFactoryProvider = ::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider();

    data->locale = locale;
    data->nouns.reserve(MAXIMUM_WORDS_TO_TEST);

    auto startRSS = PerformanceUtils_getCurrentRSS();
    auto startTime = std::chrono::high_resolution_clock::now();
    auto dictionary = ::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale);
    data->model = npc(npc(commonConceptFactoryProvider)->getCommonConceptFactory(locale))->getSemanticFeatureModel();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto endRSS = PerformanceUtils_getCurrentRSS();

    out_initTime = (int64_t)(std::chrono::duration<double, std::milli>(endTime - startTime).count());
    out_initHeap = endRSS - startRSS;
    int64_t nounPOS = 0;
    morphuntion::util::Validate::notNull(npc(dictionary)->getBinaryProperties(&nounPOS, {u"noun"}), locale.toString());

    for (const auto& word : npc(dictionary)->getKnownWords()) {
        if (!npc(dictionary)->hasAllProperties(word, nounPOS)) {
            // Uninteresting word. Most of the time we want a known noun.
            continue;
        }
        data->nouns.emplace_back(word);
        if (++data->words >= MAXIMUM_WORDS_TO_TEST) {
            // That's enough to try out.
            break;
        }
    }

    return data; 
}

int64_t InflectionSpeedPerformance(const InflectionInitData* data)
{
    auto start = std::chrono::high_resolution_clock::now();

    auto semanticFeature = data->model->getFeature(u"number");
    for (const auto& word : data->nouns) {
        //::morphuntion::dialog::InflectableStringConcept inflectableConcept(data->model, ::morphuntion::dialog::SpeakableString(word));
        ::morphuntion::dialog::SemanticConcept inflectableConcept(data->model, morphuntion::dialog::SemanticValue(u"default", word), true);
        if (semanticFeature != nullptr) {
            inflectableConcept.putConstraint(*npc(semanticFeature), u"plural");
        }
        delete inflectableConcept.toSpeakableString();
    }
    auto end = std::chrono::high_resolution_clock::now();

    return (int64_t)(std::chrono::duration<double, std::milli>(end - start).count());
}

void InflectionHeapPerformance(const InflectionInitData* data, int64_t& out_runtimeHeap, int64_t& out_peakHeap)
{
    auto start = PerformanceUtils_getCurrentRSS();

    auto semanticFeature = data->model->getFeature(u"number");
    for (const auto& word : data->nouns) {
//            ::morphuntion::dialog::InflectableStringConcept inflectableConcept(model, ::morphuntion::dialog::SpeakableString(word));
        ::morphuntion::dialog::SemanticConcept inflectableConcept(data->model, ::morphuntion::dialog::SemanticValue(u"default", word), true);
        if (semanticFeature != nullptr) {
            inflectableConcept.putConstraint(*npc(semanticFeature), u"plural");
        }
        delete inflectableConcept.toSpeakableString();
    }

    auto end = PerformanceUtils_getCurrentRSS();

    out_runtimeHeap = ((end > start) ? (end - start) : 0);
    out_peakHeap = PerformanceUtils_getPeakRSS();
}

TEST_CASE("TestInflectionPerformance#testAllLanguages", "[.]")
{
    ::std::set<::morphuntion::util::ULocale, ::std::greater<>> locales;
    auto ascendingLocales(::morphuntion::util::LocaleUtils::getSupportedLocaleList());
    locales.insert(ascendingLocales.begin(), ascendingLocales.end());
    
    auto delimiter = ",";

    PerfTable<std::ofstream> csvTable("testInflectionSpeedPerformance.csv");
    csvTable.writeRow([delimiter](std::ofstream& writer)
    {
        writer  << "locale"
                << delimiter
                << "init ms"
                << delimiter
                << "runtime ms"
                << delimiter
                << "words"
                << std::endl;
    });

    for (const auto& locale : locales) {
        int64_t initDuration, initHeap, runtimeDuration;
        InflectionInitData* data = InflectionPerformanceInitialize(locale, DEFAULT_MAXIMUM_WORDS_TO_TEST, initDuration, initHeap);

        runtimeDuration = InflectionSpeedPerformance(data);

        csvTable.writeRow([locale, delimiter, initDuration, runtimeDuration, data](std::ofstream& writer)
        {
            writer  << locale.getName()
                    << delimiter
                    << initDuration
                    << delimiter
                    << runtimeDuration
                    << delimiter
                    << data->words
                    << std::endl;
        });

        delete data;
    }
}

TEST_CASE("TestInflectionPerformance#testMemoryPressure", "[.]")
{
    ::std::set<::morphuntion::util::ULocale, ::std::greater<>> locales;
    auto ascendingLocales(::morphuntion::util::LocaleUtils::getSupportedLocaleList());
    locales.insert(ascendingLocales.begin(), ascendingLocales.end());
    
    auto delimiter = ",";

    PerfTable<std::ofstream> csvTable("testInflectionHeapPerformance.csv");
    csvTable.writeRow([delimiter](std::ofstream& writer)
    {
        writer  << "locale"
                << delimiter
                << "init. heap"
                << delimiter
                << "heap usage"
                << delimiter
                << "peak heap"
                << delimiter
                << "words"
                << ::std::endl;
    });

    for (const auto& locale : locales) {
        int64_t initDuration, initHeap, runtimeHeap, peakHeap = 0;
        InflectionInitData* data = InflectionPerformanceInitialize(locale, DEFAULT_MAXIMUM_WORDS_TO_TEST, initDuration, initHeap);

        InflectionHeapPerformance(data, runtimeHeap, peakHeap);

        csvTable.writeRow([locale, delimiter, initHeap, runtimeHeap, peakHeap, data](std::ofstream& writer)
        {
            writer  << locale.getName()
                    << delimiter
                    << initHeap
                    << delimiter
                    << runtimeHeap
                    << delimiter
                    << peakHeap
                    << delimiter
                    << data->words
                    << ::std::endl;
        });

        delete data;
    }
}
