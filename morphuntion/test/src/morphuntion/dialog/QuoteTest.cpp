/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

TEST_CASE("QuoteTest#testQuotePt")
{
    // We're verifying that region specific quotes are usable.
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::PORTUGAL());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«example»", u"foobar") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example", u"foobar")));
    factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::BRAZIL());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteFr")
{
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::FRANCE());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”", u"exampel") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example", u"exampel")));
    factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::CANADIAN_FRENCH());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteIt")
{
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::ITALIAN());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::SWITZERLAND_ITALIAN());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteDe")
{
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::GERMANY());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"„example“") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«example»") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
}

TEST_CASE("QuoteTest#testQuoteJa")
{
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::JAPAN());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”", u"exampel") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example", u"exampel")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“例子”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"例子")));
}

TEST_CASE("QuoteTest#testQuoteZhCn")
{
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::CHINA());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“例子”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"例子")));
}

TEST_CASE("QuoteTest#testQuoteYue")
{
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::HONGKONG_CHINESE());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"“example”", u"exampel") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example", u"exampel")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"「例子」") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"例子")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"「例子」", u"example") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"例子", u"example")));
}

TEST_CASE("QuoteTest#testQuoteRu")
{
    auto factory = npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider())->getCommonConceptFactory(::morphuntion::util::LocaleUtils::RUSSIA());
    REQUIRE(::morphuntion::dialog::SpeakableString(u"example") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«русский»") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"русский")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«русский»", u"Russian") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"русский", u"Russian")));
    REQUIRE(::morphuntion::dialog::SpeakableString(u"«example русский»") == npc(factory)->quote(::morphuntion::dialog::SpeakableString(u"example русский")));
}
