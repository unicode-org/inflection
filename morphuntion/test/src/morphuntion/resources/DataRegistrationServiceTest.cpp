//
//  Created by Juan Carlos Garcia on 3/24/19.
//  Copyright 2019-2024 Apple Inc. All rights reserved.
//
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/resources/DataRegistrationService.hpp>
#include <morphuntion/util/ULocale.hpp>

TEST_CASE("DataRegistrationServiceTest#testEmpty")
{
    REQUIRE(::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("zz")).empty());
}

TEST_CASE("DataRegistrationServiceTest#testSomeLanguagesPopulated")
{
    morphuntion::util::ULocale latin("la");
    morphuntion::util::ULocale churchSlavic("cu");
    morphuntion::util::ULocale ancientGreek("grc");
    auto enPath = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("en"));
    auto la = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(latin);
    ::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(latin, "file:///test/path/");
    auto cu = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(churchSlavic);
    ::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(churchSlavic, "/test/../test/path2");
    auto grc = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(ancientGreek); // Ancient Greek in Greece
    ::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(ancientGreek, "/test/path3");
    
    REQUIRE(enPath == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("en")));

    REQUIRE(::morphuntion::resources::DataRegistrationService::getDataPathForLocale(latin) == "/test/path");
    REQUIRE(::morphuntion::resources::DataRegistrationService::getDataPathForLocale(churchSlavic) == "/test/path2");
    REQUIRE(::morphuntion::resources::DataRegistrationService::getDataPathForLocale(ancientGreek) == "/test/path3");

    // reset
    CHECK_THROWS(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(latin, la));
    CHECK_NOTHROW(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(latin, "/test/path"));
    CHECK_THROWS(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(churchSlavic, cu));
    CHECK_NOTHROW(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(churchSlavic, "/test/path2"));
    CHECK_THROWS(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(ancientGreek, grc));
    CHECK_NOTHROW(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(ancientGreek, "/test/path3"));

    REQUIRE("/test/path" == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(latin));
    REQUIRE("/test/path2" == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(churchSlavic));
    REQUIRE("/test/path3" == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(ancientGreek));
}

TEST_CASE("DataRegistrationServiceTest#testSomeLocalesPopulated")
{
    auto en = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("en_US"));

    auto ia = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("ia_US")); // Interlingua
    ::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(morphuntion::util::ULocale("ia_US"), "/test/path");
    auto cu = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("arc_IQ")); // Imperial Aramaic in Iraq
    ::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(morphuntion::util::ULocale("arc_IQ"), "/test/path2");
    auto egy = ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("egy_EG")); // Ancient Egyptian in Egypt
    ::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(morphuntion::util::ULocale("egy_EG"), "/test/path3");
    
    REQUIRE(en == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("en_US")));

    REQUIRE(::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("ia_US")) == "/test/path");
    REQUIRE(::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("arc_IQ")) == "/test/path2");
    REQUIRE(::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("egy_EG")) == "/test/path3");

    // reset
    CHECK_THROWS(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(morphuntion::util::ULocale("ia_US"), ia));
    CHECK_THROWS(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(morphuntion::util::ULocale("arc_IQ"), cu));
    CHECK_THROWS(::morphuntion::resources::DataRegistrationService::registerDataPathForLocale(morphuntion::util::ULocale("egy_EG"), egy));

    REQUIRE("/test/path" == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("ia_US")));
    REQUIRE("/test/path2" == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("arc_IQ")));
    REQUIRE("/test/path3" == ::morphuntion::resources::DataRegistrationService::getDataPathForLocale(morphuntion::util::ULocale("egy_EG")));
}
