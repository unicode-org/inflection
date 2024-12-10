/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <util/CFUtils.hpp>
#include <morphuntion/util/LocaleUtils.h>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>

TEST_CASE("LocaleUtils-c#testSupportedLocaleList")
{
    auto supportedLocaleArray = mloc_getSupportedLocalesList();
    auto supportedLocaleSet(morphuntion::util::LocaleUtils::getSupportedLocaleList());
    CFIndex supportedLocaleCount = CFArrayGetCount(supportedLocaleArray);
    REQUIRE((CFIndex)supportedLocaleSet.size() == supportedLocaleCount);

    for (int32_t idx = 0; idx < supportedLocaleCount; ++idx) {
        CFLocaleRef locale = (CFLocaleRef)CFArrayGetValueAtIndex(supportedLocaleArray, idx);
        CFStringRef localeStrRef = static_cast<CFStringRef>(CFLocaleGetValue(locale, kCFLocaleIdentifier));
        auto localeStr(util::CFUtils::to_string(localeStrRef));
        REQUIRE(supportedLocaleSet.find(morphuntion::util::ULocale(localeStr)) != supportedLocaleSet.end());
    }

    CFRelease(supportedLocaleArray);
}
