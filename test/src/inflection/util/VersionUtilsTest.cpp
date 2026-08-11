/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/api.h>
#include <inflection/util/VersionUtils.hpp>

TEST_CASE("VersionUtilsTest#testVersion")
{
    std::string version = std::to_string(INFLECTION_MAJOR_VERSION) + "." + std::to_string(INFLECTION_MINOR_VERSION);
    REQUIRE(version == inflection::util::VersionUtils::getLibraryVersion());
}
