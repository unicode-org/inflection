/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/LogToString.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <algorithm>

bool contains(const ::std::vector<::std::u16string>& list, const ::std::u16string& toFind) {
    return ::std::any_of(list.begin(), list.end(), [&toFind](const auto &item){
        return (item == toFind);
    });
}

TEST_CASE("DictionaryMetaDataTest#testEnglish")
{
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::US());
    REQUIRE_FALSE(npc(dictionary)->hasProperty(u"table", u"vowel-start"));
    REQUIRE(npc(dictionary)->hasProperty(u"apple", u"noun"));
    REQUIRE(npc(dictionary)->hasProperty(u"hour", u"vowel-start"));
    REQUIRE_FALSE(npc(dictionary)->hasProperty(u"unicorn", u"vowel-start"));
    REQUIRE_FALSE(npc(dictionary)->hasProperty(u"bizzaro unknown word that does not exist in the dictionary", u"vowel-start"));
    REQUIRE_FALSE(npc(dictionary)->isKnownWord(u"bizzaro unknown word that does not exist in the dictionary"));
    REQUIRE(npc(dictionary)->isKnownWord(u"hour"));
    REQUIRE_FALSE(npc(dictionary)->getPropertyValues(u"mice", u"inflection").empty());
    REQUIRE_FALSE(npc(dictionary)->getPropertyValues(u"mouse", u"inflection").empty());
    REQUIRE_FALSE(npc(dictionary)->getPropertyValues(u"men", u"inflection").empty());
    REQUIRE_FALSE(npc(dictionary)->getPropertyValues(u"man", u"inflection").empty());
    REQUIRE_FALSE(npc(dictionary)->getPropertyValues(u"theories", u"inflection").empty());
    REQUIRE_FALSE(npc(dictionary)->getPropertyValues(u"theory", u"inflection").empty());
    REQUIRE(npc(dictionary)->hasProperty(u"Paris", u"proper-noun"));
    REQUIRE(npc(dictionary)->hasProperty(u"paris", u"proper-noun"));
    int64_t properties = 0;
    REQUIRE(npc(dictionary)->hasAnyProperty(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"noun", u"verb"}))));
    REQUIRE(npc(dictionary)->hasAnyProperty(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"noun", u"adjective"}))));
    REQUIRE(npc(dictionary)->hasAnyProperty(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"noun", u"vowel-start"}))));
    REQUIRE(npc(dictionary)->hasAnyProperty(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"plural", u"adjective"}))));
    REQUIRE(npc(dictionary)->hasAnyProperty(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"plural", u"vowel-start"}))));
    REQUIRE(npc(dictionary)->hasAllProperties(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"noun", u"adjective"}))));
    REQUIRE_FALSE(npc(dictionary)->hasAllProperties(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"noun", u"vowel-start"}))));
    REQUIRE_FALSE(npc(dictionary)->hasAllProperties(u"head", *npc(npc(dictionary)->getBinaryProperties(&properties, {u"plural", u"vowel-start"}))));
}

TEST_CASE("DictionaryMetaDataTest#testUnknown")
{
    LogToString stringLogger;
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::ULocale("tlh"));
    REQUIRE_FALSE(npc(dictionary)->isKnownWord(u"hello"));
    REQUIRE_FALSE(npc(dictionary)->isKnownWord(u"to be"));
    REQUIRE_FALSE(npc(dictionary)->isKnownWord(u"Qapla'"));
    REQUIRE_THROWS(npc(dictionary)->getPropertyValues(u"Qapla'", u"ipa").empty());
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0].find(u"[WARNING] The dictionary for tlh could not be created.") == 0);
    dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::ULocale("en_US"));

    const auto result = npc(dictionary)->getPropertyValues(u"Qapla", u"inflection");
    REQUIRE(result.size() == 0);
}

TEST_CASE("DictionaryMetaDataTest#testKorean")
{
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::KOREA());
    REQUIRE(npc(dictionary)->hasProperty(u"iPod", u"vowel-end"));
    REQUIRE(npc(dictionary)->hasProperty(u"iPhone", u"consonant-end"));
    REQUIRE(npc(dictionary)->hasProperty(u"Apple", u"rieul-end"));
    REQUIRE(npc(dictionary)->hasProperty(u"Bluetooth", u"vowel-end"));
    REQUIRE(npc(dictionary)->hasProperty(u"Abby", u"vowel-end"));
}

TEST_CASE("DictionaryMetaDataTest#testTurkish")
{
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::TURKISH());
    REQUIRE(npc(dictionary)->hasProperty(u"iPod", u"back-round"));
    REQUIRE(npc(dictionary)->hasProperty(u"iMac", u"front-unround"));
    REQUIRE(npc(dictionary)->hasProperty(u"Apple", u"back-unround"));
    REQUIRE(npc(dictionary)->hasProperty(u"Bluetooth", u"back-round"));
    REQUIRE(npc(dictionary)->hasProperty(u"Eric", u"front-unround"));
    REQUIRE(npc(dictionary)->hasProperty(u"zombie", u"vowel-end"));
    REQUIRE(npc(dictionary)->hasProperty(u"Saat", u"front-unround"));
    REQUIRE(npc(dictionary)->hasProperty(u"PTT", u"vowel-end"));
    REQUIRE(npc(dictionary)->hasProperty(u"A.Ş.", u"vowel-end"));
    REQUIRE(npc(dictionary)->hasProperty(u"YK", u"back-unround"));
    REQUIRE(npc(dictionary)->hasProperty(u"BMW", u"back-round"));
    REQUIRE(npc(dictionary)->hasProperty(u"Vodafone", u"back-round"));
    REQUIRE(npc(dictionary)->hasProperty(u"Watch", u"back-round"));
    REQUIRE(npc(dictionary)->hasProperty(u"ATM", u"front-unround"));
    REQUIRE(npc(dictionary)->hasProperty(u"Stockholm", u"front-round"));
    REQUIRE(npc(dictionary)->hasProperty(u"PDF", u"front-unround"));
}

TEST_CASE("DictionaryMetaDataTest#testRussian")
{
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::RUSSIAN());
    REQUIRE(npc(dictionary)->hasProperty(u"анатолий", u"proper-noun"));
    REQUIRE(npc(dictionary)->hasProperty(u"Анатолий", u"proper-noun"));
    REQUIRE(!npc(dictionary)->getPropertyValues(u"анатолий", u"inflection").empty());
}

TEST_CASE("DictionaryMetaDataTest#testGermanProperties")
{
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::GERMAN());
    int64_t grammemes = 0;
    REQUIRE(npc(dictionary)->getBinaryProperties(&grammemes, {u"singular", u"nominative", u"masculine", u"neuter", u"pronoun"}) != nullptr);
    REQUIRE(npc(dictionary)->hasAllProperties(u"mein", grammemes));
}

TEST_CASE("DictionaryMetaDataTest#testProperties")
{
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::ENGLISH());

    std::set<std::u16string> expectedProperties({u"consonant-start", u"noun", u"singular"});
    auto properties(npc(dictionary)->getProperties(u"unicorn"));
    REQUIRE(properties.size() == expectedProperties.size());
    for (const auto& property : properties) {
        REQUIRE(expectedProperties.find(property) != expectedProperties.end());
    }
}

TEST_CASE("DictionaryMetaDataTest#testPropertyNames")
{
    // If this triggers, then there was probably a bad parse of the dictionary.
    for (const auto& localeEntry : inflection::util::LocaleUtils::getSupportedLocaleMap()) {
        inflection::util::ULocale language(localeEntry.first);
        auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(language);
        for (const auto& propertyName : dictionary->getPropertyNames(-1)) {
            if (propertyName.find(u':') != std::u16string::npos) {
                FAIL_CHECK(language.getName() + " should not contain a colon for the property " + inflection::util::StringUtils::to_string(propertyName));
            }
        }
    }
}

TEST_CASE("DictionaryMetaDataTest#testKnownWords")
{
    auto dictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::ENGLISH());
    ::std::u16string cat(u"cat");
    ::std::u16string happyFace(u"😀");
    bool catFound = false;
    for (const auto& word : npc(dictionary)->getKnownWords()) {
        if (word == cat) {
            catFound = true;
        }
        REQUIRE(!word.empty());
        REQUIRE(word != happyFace);
    }
    REQUIRE(catFound);
}

TEST_CASE("DictionaryMetaDataTest#testFallback")
{
    auto fallbackDictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::US());
    auto fallbackWordsItr = npc(fallbackDictionary)->getKnownWords();
    REQUIRE(fallbackWordsItr != fallbackWordsItr.end());
    auto expectedDictionary = inflection::dictionary::DictionaryMetaData::createDictionary(::inflection::util::LocaleUtils::GREAT_BRITAIN());
    auto expectedWordsItr = npc(expectedDictionary)->getKnownWords();
    REQUIRE(expectedWordsItr != expectedWordsItr.end());
    REQUIRE(*fallbackWordsItr == *expectedWordsItr);
}
