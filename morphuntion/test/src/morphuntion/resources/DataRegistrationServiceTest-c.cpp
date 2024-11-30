/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/resources/DataRegistrationService.h>
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/util/ULocale.hpp>

#include "util/CFUtils.hpp"

static void checkForSuccess(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error == nullptr);
}

static void checkForFailure(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error != nullptr);
    CFRelease(*error);
    *error = nullptr;
}

TEST_CASE("DataRegistrationServiceTest-c#testAPI")
{
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::ULocale("tlh_CA"))); // Klingon in Canada.
    auto originalPath = mdr_getDataPathForLocaleCopy(locale.value, &error);
    checkForSuccess(&error);
    morphuntion::util::AutoCFRelease<CFStringRef> testPath(util::CFUtils::to_CFString(u"/test/path"));
    mdr_registerDataPathForLocale(locale.value, testPath.value, &error);
    checkForSuccess(&error);
    auto latestPath = mdr_getDataPathForLocaleCopy(locale.value, &error);
    checkForSuccess(&error);
    REQUIRE(util::CFUtils::to_u16string(latestPath) == u"/test/path");

    mdr_registerDataPathForLocale(locale.value, originalPath, &error);
    checkForFailure(&error);
    CFRelease(latestPath);
    latestPath = mdr_getDataPathForLocaleCopy(locale.value, &error);
    checkForSuccess(&error);
    REQUIRE(util::CFUtils::to_u16string(latestPath) == u"/test/path");

    CFRelease(latestPath);
    CFRelease(originalPath);
}

TEST_CASE("DataRegistrationServiceTest-c#testError") {
    CFErrorRef error = nullptr;

    mdr_getDataPathForLocaleCopy(nullptr, &error);
    checkForFailure(&error);

    mdr_registerDataPathForLocale(nullptr, nullptr, &error);
    checkForFailure(&error);
}
