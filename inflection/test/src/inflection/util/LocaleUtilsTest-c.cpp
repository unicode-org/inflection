/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/util/LocaleUtils.h>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>

static void checkForSuccess(UErrorCode* status)
{
    REQUIRE(status != nullptr);
    REQUIRE(U_SUCCESS(*status));
}

TEST_CASE("LocaleUtils-c#testSupportedLocaleList")
{
    auto error = U_ZERO_ERROR;
    auto supportedLocaleArray = iloc_getSupportedLocalesList(&error);
    checkForSuccess(&error);
    auto supportedLocaleSet(inflection::util::LocaleUtils::getSupportedLocaleList());
    CFIndex supportedLocaleCount = CFArrayGetCount(supportedLocaleArray);
    REQUIRE((CFIndex)supportedLocaleSet.size() == supportedLocaleCount);

    for (int32_t idx = 0; idx < supportedLocaleCount; ++idx) {
        const char* locale = (const char*)CFArrayGetValueAtIndex(supportedLocaleArray, idx);
        REQUIRE(supportedLocaleSet.find(inflection::util::ULocale(npc(locale))) != supportedLocaleSet.end());
    }

    CFRelease(supportedLocaleArray);
}
