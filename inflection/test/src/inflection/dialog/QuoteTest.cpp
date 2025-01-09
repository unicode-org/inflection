/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

TEST_CASE("QuoteTest#testQuotePt")
{
    // We're verifying that region specific quotes are usable.
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::PORTUGAL());
    REQUIRE(::inflection::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    REQUIRE(::inflection::dialog::SpeakableString(u"«example»", u"foobar") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example", u"foobar")));
    factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::BRAZIL());
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteFr")
{
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::FRANCE());
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”", u"exampel") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example", u"exampel")));
    factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::CANADIAN_FRENCH());
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteIt")
{
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::ITALIAN());
    REQUIRE(::inflection::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::SWITZERLAND_ITALIAN());
    REQUIRE(::inflection::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteDe")
{
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::GERMANY());
    REQUIRE(::inflection::dialog::SpeakableString(u"„example“") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::SWITZERLAND_GERMAN());
    REQUIRE(::inflection::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteJa")
{
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::JAPAN());
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”", u"exampel") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example", u"exampel")));
    REQUIRE(::inflection::dialog::SpeakableString(u"“例子”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"例子")));
}

TEST_CASE("QuoteTest#testQuoteZhCn")
{
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::CHINA());
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    REQUIRE(::inflection::dialog::SpeakableString(u"“例子”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"例子")));
}

TEST_CASE("QuoteTest#testQuoteYue")
{
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::HONGKONG_CHINESE());
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    REQUIRE(::inflection::dialog::SpeakableString(u"“example”", u"exampel") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example", u"exampel")));
    REQUIRE(::inflection::dialog::SpeakableString(u"「例子」") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"例子")));
    REQUIRE(::inflection::dialog::SpeakableString(u"「例子」", u"example") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"例子", u"example")));
}

TEST_CASE("QuoteTest#testQuoteRu")
{
    auto factory = npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::inflection::util::LocaleUtils::RUSSIA());
    REQUIRE(::inflection::dialog::SpeakableString(u"example") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example")));
    REQUIRE(::inflection::dialog::SpeakableString(u"«русский»") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"русский")));
    REQUIRE(::inflection::dialog::SpeakableString(u"«русский»", u"Russian") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"русский", u"Russian")));
    REQUIRE(::inflection::dialog::SpeakableString(u"«example русский»") == npc(factory)->quote(::inflection::dialog::SpeakableString(u"example русский")));
}
