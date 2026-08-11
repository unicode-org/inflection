/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/util/LocaleUtils.h>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>

TEST_CASE("LocaleUtils-c#testSupportedLocaleList")
{
    auto error = U_ZERO_ERROR;
    auto supportedLocaleEnum = iloc_getSupportedLocalesList(&error);
    REQUIRE(U_SUCCESS(error));
    auto supportedLocaleSet(inflection::util::LocaleUtils::getSupportedLocaleList());
    auto supportedLocaleCount = uenum_count(supportedLocaleEnum, &error);
    REQUIRE(U_SUCCESS(error));
    REQUIRE((int32_t)supportedLocaleSet.size() == supportedLocaleCount);

    for (int32_t idx = 0; idx < supportedLocaleCount; ++idx) {
        const char* locale = uenum_next(supportedLocaleEnum, nullptr, &error);
        REQUIRE(U_SUCCESS(error));
        REQUIRE(supportedLocaleSet.find(inflection::util::ULocale(npc(locale))) != supportedLocaleSet.end());
    }

    uenum_close(supportedLocaleEnum);
}
