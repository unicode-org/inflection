/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
//
// Created by George Rhoten on 2020-7-14.
//
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/lang/features/LanguageGrammarFeatures.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>
#include <atomic>
#include <barrier>
#include <memory>
#include <thread>

static void testInflection(const inflection::dialog::SemanticFeatureModel** modelPtr, const ::std::vector<::std::u16string>* phrasesPtr, const ::std::vector<::inflection::dialog::SemanticFeature>* constraintsPtr, const ::std::vector<::inflection::dialog::SemanticFeature>* semanticFeaturesPtr, std::barrier<>* barrier, const std::atomic<bool>* finished, int32_t threadID, ::std::vector<::std::unique_ptr<::inflection::dialog::SpeakableString>>* results)
{
    while (true) {
        barrier->arrive_and_wait();
        if (finished->load()) {
            break;
        }
        try {
            const inflection::dialog::SemanticFeatureModel& model = *npc(*npc(modelPtr));
            auto hasDynamicWordInflection = model.getDefaultDisplayFunction() != nullptr;
            for (const auto& phrase : *phrasesPtr) {
                inflection::dialog::InflectableStringConcept stringConcept(&model, inflection::dialog::SpeakableString(phrase));
                auto result = stringConcept.toSpeakableString();
                if (result != nullptr) {
                    results->emplace_back(result);
                }
                if (hasDynamicWordInflection) {
                    for (const auto& constraint : *constraintsPtr) {
                        for (const auto& semanticValue : constraint.getBoundedValues()) {
                            stringConcept.putConstraint(constraint, semanticValue);
                            result = stringConcept.toSpeakableString();
                            if (result != nullptr) {
                                results->emplace_back(result);
                            }
                            stringConcept.clearConstraint(constraint);
                        }
                    }
                }
                for (const auto& semanticFeature : *semanticFeaturesPtr) {
                    result = stringConcept.getFeatureValue(semanticFeature);
                    if (result != nullptr) {
                        results->emplace_back(result);
                    }
                }
            }
        }
        catch (const std::exception& e) {
            results->emplace_back(new ::inflection::dialog::SpeakableString(u"Thread exception " + inflection::util::StringUtils::to_u16string(threadID) + u": " + inflection::util::StringUtils::to_u16string(e.what())));
        }
        barrier->arrive_and_wait();
        barrier->arrive_and_wait();
        results->clear();
    }
}

TEST_CASE("DialogThreadSafetyTest#testInflect", "[multithreaded]")
{
    const int32_t processorCount = static_cast<int32_t>(std::thread::hardware_concurrency());
    REQUIRE(processorCount > 1); // This test requires at least 2 threads.
    ::std::vector<::std::thread> threads;
    threads.reserve(processorCount);
    ::inflection::util::ULocale previousLocale("");
    ::std::vector<::std::u16string> phrases;
    ::std::vector<::std::vector<::std::unique_ptr<::inflection::dialog::SpeakableString>>> results(processorCount);
    ::std::vector<inflection::dialog::SemanticFeature> constraints;
    ::std::vector<inflection::dialog::SemanticFeature> semanticFeatures;
    int32_t minimumNumberOfWords = 10;
    if (getenv("INFLECTION_TEST_EXHAUSTIVE") != nullptr) {
        // We do a quick test for development, but we can test more exhaustively if we have plenty of time to test.
        minimumNumberOfWords *= 100;
    }

    const inflection::dialog::SemanticFeatureModel* currModel = nullptr;
    std::atomic finished(false);
    std::barrier barrier(processorCount + 1);
    for (int32_t count = 0; count < processorCount; count++) {
        results[count].reserve(minimumNumberOfWords);
        threads.emplace_back(testInflection, &currModel, &phrases, &constraints, &semanticFeatures, &barrier, &finished, count, &results[count]);
    }

    for (const auto& locale : ::inflection::util::LocaleUtils::getSupportedLocaleList()) {
        if (locale.getLanguage() == previousLocale.getLanguage()) {
            // This isn't very interesting. Let's move along.
            continue;
        }
        previousLocale = locale;
        INFO(locale.getName());
        phrases.clear();
        constraints.clear();
        semanticFeatures.clear();

        inflection::dialog::SemanticFeatureModel model(locale);
        currModel = &model;
        auto features = ::inflection::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale);
        bool hasDisplayFunction = model.getDefaultDisplayFunction() != nullptr;
        for (const auto& [featureName, grammarCategory] : features.getCategories()) {
            if (featureName != u"pos" && featureName != u"sound" && grammarCategory.isUniqueValues()) {
                if (hasDisplayFunction) {
                    constraints.push_back(*npc(model.getFeature(featureName)));
                }
                // Can we query the value for gender, number, case or something like that?
                auto semanticFeature = model.getFeature(featureName);
                if (model.getDefaultFeatureFunction(*npc(semanticFeature)) != nullptr) {
                    semanticFeatures.push_back(*npc(semanticFeature));
                }
            }
            // else we don't support inflecting these other types, or they aren't interesting.
        }
        for (const auto& feature : features.getFeatures()) {
            auto semanticFeature = model.getFeature(feature.getName());
            if (model.getDefaultFeatureFunction(*npc(semanticFeature)) != nullptr) {
                semanticFeatures.push_back(*npc(semanticFeature));
            }
            // else we don't support inflecting with this semantic feature.
        }
        if (!hasDisplayFunction && semanticFeatures.empty()) {
            // Don't bother. There is nothing to inflect.
            continue;
        }

        // Random data to execute stuff outside of the dictionary.
        phrases.emplace_back(u"AAHHHHHH");
        phrases.emplace_back(u"🙂s");

        auto dictionary = ::inflection::dictionary::DictionaryMetaData::createDictionary(locale);
        int32_t wordCount = npc(dictionary)->getKnownWordsSize();
        auto dictionaryEnd = npc(dictionary)->getKnownWords().end();
        int32_t skipAmount = wordCount / minimumNumberOfWords; // Every nth entry in the dictionary
        wordCount = 0;
        for (auto dictionaryIter = npc(dictionary)->getKnownWords(); dictionaryIter != dictionaryEnd; ++dictionaryIter) {
            wordCount++;
            if (skipAmount > 0 && wordCount % skipAmount != 0) {
                continue;
            }
            phrases.emplace_back(*dictionaryIter);
        }

        barrier.arrive_and_wait(); // Signal workers to start
        barrier.arrive_and_wait(); // Wait for workers to finish

        // In all the threads that just finished, all the results should be precisely the same.
        auto numPhrases = phrases.size();
        REQUIRE(results[0].size() >= numPhrases);
        for (int32_t threadIdx = 1; threadIdx < processorCount; threadIdx++) {
            INFO("Thread=" + std::to_string(threadIdx));
            auto resultsSize = results[0].size();
            REQUIRE(results[threadIdx].size() == resultsSize);
            for (size_t resultIdx = 0; resultIdx < resultsSize; resultIdx++) {
                const auto& result = results[threadIdx].at(resultIdx);
                const auto& expected = results[0].at(resultIdx);
                if (*result != *expected) {
                    INFO(std::string("Failure at index ") + std::to_string(resultIdx));
                    FAIL(inflection::util::StringUtils::to_string(result->toString()) + "!=" + inflection::util::StringUtils::to_string(expected->toString()));
                }
            }
        }
        barrier.arrive_and_wait(); // Clear the results
    }

    finished.store(true);
    barrier.arrive_and_wait();
    for (auto& thread : threads) {
        thread.join();
    }
}
