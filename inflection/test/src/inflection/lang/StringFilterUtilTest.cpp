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

TEST_CASE("StringFilterUtilTest#testEnglishUnaccent")
{
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"a"));
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"\u0300").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"á"));
    REQUIRE(u"ab" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"São Paulo"));
    REQUIRE(u"Munchen" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"München"));
    REQUIRE(u"resume" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"résumé"));
    REQUIRE(u"Francois" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"François"));
    REQUIRE(u"東京" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::US(), u"東京"));
}

TEST_CASE("StringFilterUtilTest#testGermanUnaccent")
{
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"a"));
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"\u0300").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"á"));
    REQUIRE(u"ab" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"São Paulo"));
    REQUIRE(u"München" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"München"));
    REQUIRE(u"resume" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"résumé"));
    REQUIRE(u"Francois" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::GERMANY(), u"François"));
}

TEST_CASE("StringFilterUtilTest#testFrenchUnaccent")
{
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"a"));
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"\u0300").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"á"));
    REQUIRE(u"ab" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"São Paulo"));
    REQUIRE(u"München" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"München"));
    REQUIRE(u"résumé" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"résumé"));
    REQUIRE(u"François" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::FRANCE(), u"François"));
}

TEST_CASE("StringFilterUtilTest#testJapaneseUnaccent")
{
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"a"));
    REQUIRE(::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"\u0300").empty());
    REQUIRE(u"a" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"á"));
    REQUIRE(u"ab" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"São Paulo"));
    REQUIRE(u"Munchen" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"München"));
    REQUIRE(u"resume" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"résumé"));
    REQUIRE(u"Francois" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"François"));
    REQUIRE(u"東京" == ::inflection::lang::StringFilterUtil::stripNonNativeAccents(::inflection::util::LocaleUtils::JAPAN(), u"東京"));
}

TEST_CASE("StringFilterUtilTest#testUnaccent")
{
    ::std::u16string result;
    REQUIRE(npc(::inflection::lang::StringFilterUtil::unaccent(&result, u""))->empty());
    REQUIRE(u"a" == *npc(::inflection::lang::StringFilterUtil::unaccent(&result, u"a")));
    REQUIRE(npc(::inflection::lang::StringFilterUtil::unaccent(&result, u"\u0300"))->empty());
    REQUIRE(u"a" == *npc(::inflection::lang::StringFilterUtil::unaccent(&result, u"á")));
    REQUIRE(u"ab" == *npc(::inflection::lang::StringFilterUtil::unaccent(&result, u"\u0300áb")));
    REQUIRE(u"ab🐰c" == *npc(::inflection::lang::StringFilterUtil::unaccent(&result, u"áb🐰c")));
}

TEST_CASE("StringFilterUtilTest#testEqualIgnoreCaseAndAllAccents")
{
    REQUIRE(::inflection::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"Istanbul", u"istanbul", ::inflection::util::LocaleUtils::ENGLISH()));
    REQUIRE(::inflection::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"resume", u"Résumé", ::inflection::util::LocaleUtils::ENGLISH()));
    REQUIRE(!::inflection::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"Istanbul", u"istanbul", ::inflection::util::LocaleUtils::TURKISH()));
    REQUIRE(::inflection::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"Istanbul", u"ıstanbul", ::inflection::util::LocaleUtils::TURKISH()));
    REQUIRE(::inflection::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"İstanbul", u"istanbul", ::inflection::util::LocaleUtils::TURKISH()));
}
