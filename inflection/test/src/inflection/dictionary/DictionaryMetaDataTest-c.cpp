/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/DictionaryMetaData.h>
#include <inflection/util/AutoCRelease.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <set>

static void checkForFailure(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_FAILURE(*status));
    *status = U_ZERO_ERROR;
}

std::vector<std::u16string> convertToPropertyNames(const IDDictionaryMetaData* dictionary, int64_t value)
{
    std::vector<std::u16string> result;
    char16_t name[64] = {  };
    while (value != 0) {
        const auto singleProperty = ((int64_t)(((uint64_t)(value ^ (value - 1))) >> 1)) + 1;
        value &= value - 1;
        auto error = U_ZERO_ERROR;
        int len = iddmd_getPropertyName(dictionary, singleProperty, name, std::size(name), &error);
        REQUIRE(U_SUCCESS(error));
        result.emplace_back(name, len);
    }
    return result;
}

TEST_CASE("DictionaryMetaDataTest-c#testProperties"){
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto dictionary = iddmd_getDictionary(locale, &error);
    REQUIRE(U_SUCCESS(error));
    std::set<std::u16string> expectedProperties({u"consonant-start", u"noun", u"singular"});
    int64_t properties = 0;
    REQUIRE(iddmd_getCombinedBinaryType(dictionary, &properties, u"unicorn", -1, &error) != nullptr);
    REQUIRE(U_SUCCESS(error));
    auto propertyNames(convertToPropertyNames(dictionary, properties));
    REQUIRE(propertyNames.size() == expectedProperties.size());
    for (const auto& property : propertyNames) {
        REQUIRE(expectedProperties.find(property) != expectedProperties.end());
    }
}

TEST_CASE("DictionaryMetaDataTest-c#testEnglish")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto dictionary = iddmd_getDictionary(locale, &error);
    REQUIRE(U_SUCCESS(error));
    REQUIRE(iddmd_isKnownWord(dictionary, u"hour", -1, &error));
    REQUIRE(U_SUCCESS(error));
}

TEST_CASE("DictionaryMetaDataTest-c#testAPI")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto dictionary = iddmd_getDictionary(locale, &error);
    REQUIRE(U_SUCCESS(error));
    char16_t singularStrArr[] = u"singular";
    const char16_t* propArray[] = {u"singular"};
    auto singularBitMask(iddmd_getBinaryProperties(dictionary, propArray, std::size(propArray), &error));
    REQUIRE(U_SUCCESS(error));
    char16_t singularString[std::size(singularStrArr)] = {  };
    iddmd_getPropertyName(dictionary, singularBitMask, singularString, std::size(singularString), &error);
    REQUIRE(std::u16string_view(singularString) == u"singular");
}

TEST_CASE("DictionaryMetaDataTest-c#testError")
{
    auto error = U_ZERO_ERROR;

    iddmd_getCombinedBinaryType(nullptr, nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    iddmd_getPropertyName(nullptr, -1, nullptr, -1, &error);
    checkForFailure(&error);
    iddmd_isKnownWord(nullptr, nullptr, -1, &error);
    checkForFailure(&error);

    iddmd_getBinaryProperties(nullptr, nullptr, -1, &error);
    checkForFailure(&error);
    iddmd_getDictionary(nullptr, &error);
    checkForFailure(&error);
}
