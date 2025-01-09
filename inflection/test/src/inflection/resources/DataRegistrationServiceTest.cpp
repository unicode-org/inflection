//
//  Created by Juan Carlos Garcia on 3/24/19.
//  Copyright 2019-2024 Apple Inc. All rights reserved.
//
#include "catch2/catch_test_macros.hpp"

#include <inflection/resources/DataRegistrationService.hpp>
#include <inflection/util/ULocale.hpp>

TEST_CASE("DataRegistrationServiceTest#testEmpty")
{
    REQUIRE(::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("zz")).empty());
}

TEST_CASE("DataRegistrationServiceTest#testSomeLanguagesPopulated")
{
    inflection::util::ULocale latin("la");
    inflection::util::ULocale churchSlavic("cu");
    inflection::util::ULocale ancientGreek("grc");
    auto enPath = ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("en"));
    auto la = ::inflection::resources::DataRegistrationService::getDataPathForLocale(latin);
    ::inflection::resources::DataRegistrationService::registerDataPathForLocale(latin, "file:///test/path/");
    auto cu = ::inflection::resources::DataRegistrationService::getDataPathForLocale(churchSlavic);
    ::inflection::resources::DataRegistrationService::registerDataPathForLocale(churchSlavic, "/test/../test/path2");
    auto grc = ::inflection::resources::DataRegistrationService::getDataPathForLocale(ancientGreek); // Ancient Greek in Greece
    ::inflection::resources::DataRegistrationService::registerDataPathForLocale(ancientGreek, "/test/path3");
    
    REQUIRE(enPath == ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("en")));

    REQUIRE(::inflection::resources::DataRegistrationService::getDataPathForLocale(latin) == "/test/path");
    REQUIRE(::inflection::resources::DataRegistrationService::getDataPathForLocale(churchSlavic) == "/test/path2");
    REQUIRE(::inflection::resources::DataRegistrationService::getDataPathForLocale(ancientGreek) == "/test/path3");

    // reset
    CHECK_THROWS(::inflection::resources::DataRegistrationService::registerDataPathForLocale(latin, la));
    CHECK_NOTHROW(::inflection::resources::DataRegistrationService::registerDataPathForLocale(latin, "/test/path"));
    CHECK_THROWS(::inflection::resources::DataRegistrationService::registerDataPathForLocale(churchSlavic, cu));
    CHECK_NOTHROW(::inflection::resources::DataRegistrationService::registerDataPathForLocale(churchSlavic, "/test/path2"));
    CHECK_THROWS(::inflection::resources::DataRegistrationService::registerDataPathForLocale(ancientGreek, grc));
    CHECK_NOTHROW(::inflection::resources::DataRegistrationService::registerDataPathForLocale(ancientGreek, "/test/path3"));

    REQUIRE("/test/path" == ::inflection::resources::DataRegistrationService::getDataPathForLocale(latin));
    REQUIRE("/test/path2" == ::inflection::resources::DataRegistrationService::getDataPathForLocale(churchSlavic));
    REQUIRE("/test/path3" == ::inflection::resources::DataRegistrationService::getDataPathForLocale(ancientGreek));
}

TEST_CASE("DataRegistrationServiceTest#testSomeLocalesPopulated")
{
    auto en = ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("en_US"));

    auto ia = ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("ia_US")); // Interlingua
    ::inflection::resources::DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale("ia_US"), "/test/path");
    auto cu = ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("arc_IQ")); // Imperial Aramaic in Iraq
    ::inflection::resources::DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale("arc_IQ"), "/test/path2");
    auto egy = ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("egy_EG")); // Ancient Egyptian in Egypt
    ::inflection::resources::DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale("egy_EG"), "/test/path3");
    
    REQUIRE(en == ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("en_US")));

    REQUIRE(::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("ia_US")) == "/test/path");
    REQUIRE(::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("arc_IQ")) == "/test/path2");
    REQUIRE(::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("egy_EG")) == "/test/path3");

    // reset
    CHECK_THROWS(::inflection::resources::DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale("ia_US"), ia));
    CHECK_THROWS(::inflection::resources::DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale("arc_IQ"), cu));
    CHECK_THROWS(::inflection::resources::DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale("egy_EG"), egy));

    REQUIRE("/test/path" == ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("ia_US")));
    REQUIRE("/test/path2" == ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("arc_IQ")));
    REQUIRE("/test/path3" == ::inflection::resources::DataRegistrationService::getDataPathForLocale(inflection::util::ULocale("egy_EG")));
}
