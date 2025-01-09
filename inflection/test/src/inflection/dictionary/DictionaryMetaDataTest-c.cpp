/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dictionary/DictionaryMetaData.h>
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include "util/CFUtils.hpp"
#include <set>


using ::util::TypeConversionUtils;

static void checkForSuccess(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_SUCCESS(*status));
}

static void checkForFailure(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_FAILURE(*status));
    *status = U_ZERO_ERROR;
}

TEST_CASE("DictionaryMetaDataTest-c#testProperties"){
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto dictionary = iddmd_getDictionary(locale, &error);
    checkForSuccess(&error);
    std::set<std::u16string> expectedProperties({u"consonant-start", u"noun", u"singular"});
    CFStringRef word = TypeConversionUtils::to_CFString(u"unicorn");
    int64_t properties = 0;
    REQUIRE(iddmd_getCombinedBinaryType(dictionary, &properties, word, &error) != nullptr);
    checkForSuccess(&error);
    CFRelease(word);
    auto propertyNameCFArray = iddmd_createPropertyNames(dictionary, properties, &error);
    checkForSuccess(&error);
    auto propertyNames(TypeConversionUtils::to_u16stringVector(propertyNameCFArray));
    CFRelease(propertyNameCFArray);
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
    checkForSuccess(&error);
    CFStringRef word = TypeConversionUtils::to_CFString(u"hour");
    REQUIRE(iddmd_isKnownWord(dictionary, word, &error));
    CFRelease(word);
    checkForSuccess(&error);
}

TEST_CASE("DictionaryMetaDataTest-c#testAPI")
{
    auto error = U_ZERO_ERROR;
    auto locale = ::inflection::util::LocaleUtils::ENGLISH().getName().c_str();
    auto dictionary = iddmd_getDictionary(locale, &error);
    checkForSuccess(&error);
    CFArrayRef propArray = TypeConversionUtils::to_CFArray({u"singular"});
    auto singularBitMask(iddmd_getBinaryProperties(dictionary, propArray, &error));
    checkForSuccess(&error);
    CFRelease(propArray);
    auto singularCFString = iddmd_createPropertyName(dictionary, singularBitMask, &error);
    auto singularString = TypeConversionUtils::to_u16string(singularCFString);
    CFRelease(singularCFString);
    REQUIRE(singularString == u"singular");
}

TEST_CASE("DictionaryMetaDataTest-c#testError")
{
    auto error = U_ZERO_ERROR;

    iddmd_getCombinedBinaryType(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
    iddmd_createPropertyNames(nullptr, -1, &error);
    checkForFailure(&error);
    iddmd_createPropertyName(nullptr, -1, &error);
    checkForFailure(&error);
    iddmd_isKnownWord(nullptr, nullptr, &error);
    checkForFailure(&error);

    iddmd_getBinaryProperties(nullptr, nullptr, &error);
    checkForFailure(&error);
    iddmd_getDictionary(nullptr, &error);
    checkForFailure(&error);
}
