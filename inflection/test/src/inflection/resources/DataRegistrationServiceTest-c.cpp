/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/resources/DataRegistrationService.h>
#include <inflection/util/ULocale.hpp>

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
    char originalPath[4096] = {};
    char latestPath[4096] = {};
    idr_getDataPathForLocale(locale, originalPath, sizeof(originalPath), &error);
    REQUIRE(U_SUCCESS(error));
    idr_registerDataPathForLocale(locale, "/test/path", &error);
    REQUIRE(U_SUCCESS(error));
    idr_getDataPathForLocale(locale, latestPath, sizeof(latestPath), &error);
    REQUIRE(U_SUCCESS(error));
    REQUIRE(std::string(latestPath) == "/test/path");

    idr_registerDataPathForLocale(locale, originalPath, &error);
    checkForFailure(&error);
    idr_getDataPathForLocale(locale, latestPath, sizeof(latestPath), &error);
    REQUIRE(U_SUCCESS(error));
    REQUIRE(std::string(latestPath) == "/test/path");
}

TEST_CASE("DataRegistrationServiceTest-c#testError") {
    auto error = U_ZERO_ERROR;

    idr_getDataPathForLocale(nullptr, nullptr, -1, &error);
    checkForFailure(&error);

    idr_registerDataPathForLocale(nullptr, nullptr, &error);
    checkForFailure(&error);
}
