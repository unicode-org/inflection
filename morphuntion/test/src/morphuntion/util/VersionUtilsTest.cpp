/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/api.h>
#include <morphuntion/util/VersionUtils.hpp>

TEST_CASE("VersionUtilsTest#testVersion")
{
    std::string version = std::to_string(MORPHUNTION_MAJOR_VERSION) + "." + std::to_string(MORPHUNTION_MINOR_VERSION);
    REQUIRE(version == morphuntion::util::VersionUtils::getLibraryVersion());
}
