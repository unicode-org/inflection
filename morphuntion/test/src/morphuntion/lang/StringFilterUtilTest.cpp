/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

TEST_CASE("StringFilterUtilTest#testHasCJKCharacter")
{
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"abcにある"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"にある"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"にあるabc"));
    REQUIRE(!::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"abc"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"🐰る"));
    REQUIRE(!::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"🐰"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"abc在"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"在"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"在a"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"🐰在"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"근처"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(u"ก"));
}

TEST_CASE("StringFilterUtilTest#testIsFirstCJKCharacter")
{
    REQUIRE(!::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"abcにある"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"にある"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"に"));
    REQUIRE(!::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"Michele話"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"にあるabc"));
    REQUIRE(!::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"a"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"𡑮"));
    REQUIRE(!::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"🐰る"));
    REQUIRE(!::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(u"🐰"));
}

TEST_CASE("StringFilterUtilTest#testEnglishUnaccent")
{
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"a"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"\u0300").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"á"));
    REQUIRE(u"ab" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"São Paulo"));
    REQUIRE(u"Munchen" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"München"));
    REQUIRE(u"resume" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"résumé"));
    REQUIRE(u"Francois" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"François"));
    REQUIRE(u"東京" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::US(), u"東京"));
}

TEST_CASE("StringFilterUtilTest#testGermanUnaccent")
{
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"a"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"\u0300").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"á"));
    REQUIRE(u"ab" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"São Paulo"));
    REQUIRE(u"München" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"München"));
    REQUIRE(u"resume" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"résumé"));
    REQUIRE(u"Francois" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::GERMANY(), u"François"));
}

TEST_CASE("StringFilterUtilTest#testFrenchUnaccent")
{
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"a"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"\u0300").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"á"));
    REQUIRE(u"ab" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"São Paulo"));
    REQUIRE(u"München" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"München"));
    REQUIRE(u"résumé" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"résumé"));
    REQUIRE(u"François" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::FRANCE(), u"François"));
}

TEST_CASE("StringFilterUtilTest#testJapaneseUnaccent")
{
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"a"));
    REQUIRE(::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"\u0300").empty());
    REQUIRE(u"a" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"á"));
    REQUIRE(u"ab" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"\u0300áb"));
    REQUIRE(u"ab🐰c" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"áb🐰c"));
    REQUIRE(u"Sao Paulo" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"São Paulo"));
    REQUIRE(u"Munchen" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"München"));
    REQUIRE(u"resume" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"résumé"));
    REQUIRE(u"Francois" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"François"));
    REQUIRE(u"東京" == ::morphuntion::lang::StringFilterUtil::stripNonNativeAccents(::morphuntion::util::LocaleUtils::JAPAN(), u"東京"));
}

TEST_CASE("StringFilterUtilTest#testUnaccent")
{
    ::std::u16string result;
    REQUIRE(npc(::morphuntion::lang::StringFilterUtil::unaccent(&result, u""))->empty());
    REQUIRE(u"a" == *npc(::morphuntion::lang::StringFilterUtil::unaccent(&result, u"a")));
    REQUIRE(npc(::morphuntion::lang::StringFilterUtil::unaccent(&result, u"\u0300"))->empty());
    REQUIRE(u"a" == *npc(::morphuntion::lang::StringFilterUtil::unaccent(&result, u"á")));
    REQUIRE(u"ab" == *npc(::morphuntion::lang::StringFilterUtil::unaccent(&result, u"\u0300áb")));
    REQUIRE(u"ab🐰c" == *npc(::morphuntion::lang::StringFilterUtil::unaccent(&result, u"áb🐰c")));
}

TEST_CASE("StringFilterUtilTest#testEqualIgnoreCaseAndAllAccents")
{
    REQUIRE(::morphuntion::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"Istanbul", u"istanbul", ::morphuntion::util::LocaleUtils::ENGLISH()));
    REQUIRE(::morphuntion::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"resume", u"Résumé", ::morphuntion::util::LocaleUtils::ENGLISH()));
    REQUIRE(!::morphuntion::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"Istanbul", u"istanbul", ::morphuntion::util::LocaleUtils::TURKISH()));
    REQUIRE(::morphuntion::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"Istanbul", u"ıstanbul", ::morphuntion::util::LocaleUtils::TURKISH()));
    REQUIRE(::morphuntion::lang::StringFilterUtil::equalIgnoreCaseAndAllAccents(u"İstanbul", u"istanbul", ::morphuntion::util::LocaleUtils::TURKISH()));
}
