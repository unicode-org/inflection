/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dictionary/DictionaryMetaData.h>
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include "util/CFUtils.hpp"
#include <set>


using ::util::CFUtils;

static void checkForSuccess(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error == nullptr);
}

TEST_CASE("DictionaryMetaDataTest-c#testProperties"){
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));
    auto dictionary = mddmd_getDictionary(locale.value, &error);
    checkForSuccess(&error);
    std::set<std::u16string> expectedProperties({u"consonant-start", u"noun", u"singular"});
    CFStringRef word = CFUtils::to_CFString(u"unicorn");
    int64_t properties = 0;
    REQUIRE(mddmd_getCombinedBinaryType(dictionary, &properties, word, &error) != nullptr);
    checkForSuccess(&error);
    CFRelease(word);
    auto propertyNameCFArray = mddmd_createPropertyNames(dictionary, properties, &error);
    checkForSuccess(&error);
    auto propertyNames(CFUtils::to_u16stringVector(propertyNameCFArray));
    CFRelease(propertyNameCFArray);
    REQUIRE(propertyNames.size() == expectedProperties.size());
    for (const auto& property : propertyNames) {
        REQUIRE(expectedProperties.find(property) != expectedProperties.end());
    }
}

TEST_CASE("DictionaryMetaDataTest-c#testEnglish")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));
    auto dictionary = mddmd_getDictionary(locale.value, &error);
    checkForSuccess(&error);
    CFStringRef word = CFUtils::to_CFString(u"hour");
    REQUIRE(mddmd_isKnownWord(dictionary, word, &error));
    CFRelease(word);
    checkForSuccess(&error);
}

TEST_CASE("DictionaryMetaDataTest-c#testAPI")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::LocaleUtils::ENGLISH()));
    auto dictionary = mddmd_getDictionary(locale.value, &error);
    checkForSuccess(&error);
    CFArrayRef propArray = CFUtils::to_CFArray({u"singular"});
    auto singularBitMask(mddmd_getBinaryProperties(dictionary, propArray, &error));
    checkForSuccess(&error);
    CFRelease(propArray);
    auto singularCFString = mddmd_createPropertyName(dictionary, singularBitMask, &error);
    auto singularString = CFUtils::to_u16string(singularCFString);
    CFRelease(singularCFString);
    REQUIRE(singularString == u"singular");
}

static void checkForFailure(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error != nullptr);
    CFRelease(*error);
    *error = nullptr;
}

TEST_CASE("DictionaryMetaDataTest-c#testError")
{
    CFErrorRef error = nullptr;

    mddmd_getCombinedBinaryType(nullptr, nullptr, nullptr, &error);
    checkForFailure(&error);
    mddmd_createPropertyNames(nullptr, -1, &error);
    checkForFailure(&error);
    mddmd_createPropertyName(nullptr, -1, &error);
    checkForFailure(&error);
    mddmd_isKnownWord(nullptr, nullptr, &error);
    checkForFailure(&error);

    mddmd_getBinaryProperties(nullptr, nullptr, &error);
    checkForFailure(&error);
    mddmd_getDictionary(nullptr, &error);
    checkForFailure(&error);
}
