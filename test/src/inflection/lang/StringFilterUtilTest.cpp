/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

TEST_CASE("StringFilterUtilTest#testHasCJKCharacter")
{
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"abcにある"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"にある"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"にあるabc"));
    REQUIRE(!::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"abc"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"🐰る"));
    REQUIRE(!::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"🐰"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"abc在"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"在"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"在a"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"🐰在"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"근처"));
    REQUIRE(::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"ก"));
}

TEST_CASE("StringFilterUtilTest#testIsFirstCJKCharacter")
{
    REQUIRE(!::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"abcにある"));
    REQUIRE(::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"にある"));
    REQUIRE(::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"に"));
    REQUIRE(!::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"Michele話"));
    REQUIRE(::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"にあるabc"));
    REQUIRE(!::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"a"));
    REQUIRE(::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"𡑮"));
    REQUIRE(!::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"🐰る"));
    REQUIRE(!::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"🐰"));
}
