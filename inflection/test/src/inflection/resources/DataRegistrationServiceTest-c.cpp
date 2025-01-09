/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/resources/DataRegistrationService.h>
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/util/ULocale.hpp>

#include "util/CFUtils.hpp"

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

TEST_CASE("DataRegistrationServiceTest-c#testAPI")
{
    auto error = U_ZERO_ERROR;
    auto locale = "tlh_CA"; // Klingon in Canada.
    auto originalPath = idr_getDataPathForLocaleCopy(locale, &error);
    checkForSuccess(&error);
    inflection::util::AutoCFRelease<CFStringRef> testPath(util::TypeConversionUtils::to_CFString(u"/test/path"));
    idr_registerDataPathForLocale(locale, testPath.value, &error);
    checkForSuccess(&error);
    auto latestPath = idr_getDataPathForLocaleCopy(locale, &error);
    checkForSuccess(&error);
    REQUIRE(util::TypeConversionUtils::to_u16string(latestPath) == u"/test/path");

    idr_registerDataPathForLocale(locale, originalPath, &error);
    checkForFailure(&error);
    CFRelease(latestPath);
    latestPath = idr_getDataPathForLocaleCopy(locale, &error);
    checkForSuccess(&error);
    REQUIRE(util::TypeConversionUtils::to_u16string(latestPath) == u"/test/path");

    CFRelease(latestPath);
    CFRelease(originalPath);
}

TEST_CASE("DataRegistrationServiceTest-c#testError") {
    auto error = U_ZERO_ERROR;

    idr_getDataPathForLocaleCopy(nullptr, &error);
    checkForFailure(&error);

    idr_registerDataPathForLocale(nullptr, nullptr, &error);
    checkForFailure(&error);
}
