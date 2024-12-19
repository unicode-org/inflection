/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/LogToString.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <unicode/uloc.h>
#include <set>

TEST_CASE("LocaleUtilsTest#testCreateValidRegionAwareLanguage")
{
    REQUIRE(::morphuntion::util::LocaleUtils::SAUDI_ARABIA() == ::morphuntion::util::LocaleUtils::createValidRegionAwareLanguage(::morphuntion::util::LocaleUtils::SAUDI_ARABIA(), ::morphuntion::util::LocaleUtils::US()));
    REQUIRE(::morphuntion::util::ULocale("ar_TN") == ::morphuntion::util::LocaleUtils::createValidRegionAwareLanguage(::morphuntion::util::LocaleUtils::SAUDI_ARABIA(), ::morphuntion::util::ULocale("ar_TN")));
    // Stay the same script. Only change the region.
    REQUIRE(::morphuntion::util::ULocale("sr_Cyrl_BA") == ::morphuntion::util::LocaleUtils::createValidRegionAwareLanguage(::morphuntion::util::ULocale("sr_Cyrl_RS"), ::morphuntion::util::ULocale("sr_Latn_BA")));
    REQUIRE(::morphuntion::util::ULocale("sr_Cyrl_RS") == ::morphuntion::util::LocaleUtils::createValidRegionAwareLanguage(::morphuntion::util::ULocale("sr_Cyrl_RS"), ::morphuntion::util::ULocale("sr_Latn_RS")));
    REQUIRE(::morphuntion::util::ULocale("es_CO") == ::morphuntion::util::LocaleUtils::createValidRegionAwareLanguage(::morphuntion::util::ULocale("es_MX"), ::morphuntion::util::ULocale("es_CO")));
    REQUIRE(::morphuntion::util::ULocale("es_MX") == ::morphuntion::util::LocaleUtils::createValidRegionAwareLanguage(::morphuntion::util::ULocale("es_CO"), ::morphuntion::util::ULocale("es_MX")));
}

TEST_CASE("LocaleUtilsTest#testLanguageScriptCompatible")
{
    REQUIRE(::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::US()));
    REQUIRE(::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::GREAT_BRITAIN()));
    REQUIRE(!::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::US(), ::morphuntion::util::LocaleUtils::GERMANY()));
    REQUIRE(::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::CHINA(), ::morphuntion::util::LocaleUtils::CHINA()));
    REQUIRE(::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::TAIWAN(), ::morphuntion::util::LocaleUtils::HONGKONG_CHINESE()));
    REQUIRE(::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::CHINA(), ::morphuntion::util::LocaleUtils::CHINA_CANTONESE()));
    REQUIRE(!::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::CHINA(), ::morphuntion::util::LocaleUtils::TAIWAN()));
    REQUIRE(!::morphuntion::util::LocaleUtils::areLanguageScriptCompatible(::morphuntion::util::LocaleUtils::TAIWAN(), ::morphuntion::util::LocaleUtils::CHINA_CANTONESE()));
}

TEST_CASE("LocaleUtilsTest#testToString")
{
    REQUIRE(u"zh_CN" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("zh", "CN")));
    REQUIRE(u"en" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("en")));
    REQUIRE(u"zh_CN" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("zh", "CN")));
    REQUIRE(u"he" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("he")));
    REQUIRE(u"he" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::LocaleUtils::toLocale(u"he")));
    REQUIRE(u"he_IL" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("he", "IL")));
    REQUIRE(u"iw_IL" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("iw", "IL")));
    REQUIRE(u"zh_Hans_CN" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("zh", "hans", "cn")));
    REQUIRE(u"zh-Hans-CN" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("zh", "hans", "cn"), u"-"));
    REQUIRE(u"en" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("en"), u"-"));
    REQUIRE(u"en/US" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("en", "us"), u"/"));
    REQUIRE("vi_VN@sd=vnct" == ::morphuntion::util::ULocale("vi", "VN", "sd=vnct").getName());
    REQUIRE("vi_VN@sd=vnct" == ::morphuntion::util::ULocale("vi", "VN", "", "sd=vnct").getName());
    REQUIRE(u"vi_VN_sd=vnct" == ::morphuntion::util::LocaleUtils::toString(::morphuntion::util::ULocale("vi", "VN", "", "sd=vnct")));
}

TEST_CASE("LocaleUtilsTest#hashTest")
{
    morphuntion::util::ULocale locale1 = morphuntion::util::LocaleUtils::ARABIC();
    morphuntion::util::ULocale locale2 = morphuntion::util::LocaleUtils::GERMAN();

    std::set<morphuntion::util::ULocale> s;
    s.insert(locale1);
    s.insert(locale2);

    auto it = s.begin();

    REQUIRE(locale2 == *it);
    REQUIRE(locale1 == *(++it));
}

TEST_CASE("LocaleUtilsTest#testCoverage")
{
    // Ensure that the constants and the dynamic list sync up.
    ::std::set<::morphuntion::util::ULocale> languages({
        morphuntion::util::LocaleUtils::ARABIC(),
        morphuntion::util::LocaleUtils::BULGARIAN(),
        morphuntion::util::LocaleUtils::CATALAN(),
        morphuntion::util::LocaleUtils::CZECH(),
        morphuntion::util::LocaleUtils::DANISH(),
        morphuntion::util::LocaleUtils::GERMAN(),
        morphuntion::util::LocaleUtils::GREEK(),
        morphuntion::util::LocaleUtils::ENGLISH(),
        morphuntion::util::LocaleUtils::SPANISH(),
        morphuntion::util::LocaleUtils::ESTONIAN(),
        morphuntion::util::LocaleUtils::FINNISH(),
        morphuntion::util::LocaleUtils::FRENCH(),
        morphuntion::util::LocaleUtils::HEBREW(),
        morphuntion::util::LocaleUtils::HINDI(),
        morphuntion::util::LocaleUtils::CROATIAN(),
        morphuntion::util::LocaleUtils::HUNGARIAN(),
        morphuntion::util::LocaleUtils::INDONESIAN(),
        morphuntion::util::LocaleUtils::ICELANDIC(),
        morphuntion::util::LocaleUtils::ITALIAN(),
        morphuntion::util::LocaleUtils::JAPANESE(),
        morphuntion::util::LocaleUtils::KAZAKH(),
        morphuntion::util::LocaleUtils::KOREAN(),
        morphuntion::util::LocaleUtils::LITHUANIAN(),
        morphuntion::util::LocaleUtils::MALAY(),
        morphuntion::util::LocaleUtils::NORWEGIAN(),
        morphuntion::util::LocaleUtils::DUTCH(),
        morphuntion::util::LocaleUtils::POLISH(),
        morphuntion::util::LocaleUtils::PORTUGUESE(),
        morphuntion::util::LocaleUtils::ROMANIAN(),
        morphuntion::util::LocaleUtils::RUSSIAN(),
        morphuntion::util::LocaleUtils::SERBIAN(),
        morphuntion::util::LocaleUtils::SLOVAK(),
        morphuntion::util::LocaleUtils::SWEDISH(),
        morphuntion::util::LocaleUtils::THAI(),
        morphuntion::util::LocaleUtils::TURKISH(),
        morphuntion::util::LocaleUtils::UKRAINIAN(),
        morphuntion::util::LocaleUtils::VIETNAMESE(),
        morphuntion::util::LocaleUtils::CANTONESE(),
        morphuntion::util::LocaleUtils::CHINESE(),
    });
    ::std::set<::morphuntion::util::ULocale> locales({
        morphuntion::util::LocaleUtils::SAUDI_ARABIA(),
        morphuntion::util::LocaleUtils::BULGARIA(),
        morphuntion::util::LocaleUtils::SPAIN_CATALAN(),
        morphuntion::util::LocaleUtils::CZECHIA(),
        morphuntion::util::LocaleUtils::DENMARK(),
        morphuntion::util::LocaleUtils::GERMANY(),
        morphuntion::util::LocaleUtils::AUSTRIA(),
        morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN(),
        morphuntion::util::LocaleUtils::GREECE(),
        morphuntion::util::LocaleUtils::AUSTRALIA_ENGLISH(),
        morphuntion::util::LocaleUtils::CANADIAN_ENGLISH(),
        morphuntion::util::LocaleUtils::GREAT_BRITAIN(),
        morphuntion::util::LocaleUtils::IRISH_ENGLISH(),
        morphuntion::util::LocaleUtils::INDIAN_ENGLISH(),
        morphuntion::util::LocaleUtils::NEW_ZEALAND_ENGLISH(),
        morphuntion::util::LocaleUtils::SINGAPOREAN_ENGLISH(),
        morphuntion::util::LocaleUtils::US(),
        morphuntion::util::LocaleUtils::SOUTH_AFRICAN_ENGLISH(),
        morphuntion::util::LocaleUtils::CHILEAN_SPANISH(),
        morphuntion::util::LocaleUtils::SPAIN_SPANISH(),
        morphuntion::util::LocaleUtils::MEXICO_SPANISH(),
        morphuntion::util::LocaleUtils::US_SPANISH(),
        morphuntion::util::LocaleUtils::ESTONIA(),
        morphuntion::util::LocaleUtils::FINLAND(),
        morphuntion::util::LocaleUtils::BELGIUM_FRENCH(),
        morphuntion::util::LocaleUtils::CANADIAN_FRENCH(),
        morphuntion::util::LocaleUtils::FRANCE(),
        morphuntion::util::LocaleUtils::SWITZERLAND_FRENCH(),
        morphuntion::util::LocaleUtils::INDIA_HINDI(),
        morphuntion::util::LocaleUtils::CROATIA(),
        morphuntion::util::LocaleUtils::ISRAEL(),
        morphuntion::util::LocaleUtils::HUNGARY(),
        morphuntion::util::LocaleUtils::INDONESIA(),
        morphuntion::util::LocaleUtils::ICELAND(),
        morphuntion::util::LocaleUtils::SWITZERLAND_ITALIAN(),
        morphuntion::util::LocaleUtils::ITALY(),
        morphuntion::util::LocaleUtils::JAPAN(),
        morphuntion::util::LocaleUtils::KAZAKHSTAN(),
        morphuntion::util::LocaleUtils::KOREA(),
        morphuntion::util::LocaleUtils::LITHUANIA(),
        morphuntion::util::LocaleUtils::MALAYSIA(),
        morphuntion::util::LocaleUtils::NORWAY(),
        morphuntion::util::LocaleUtils::BELGIUM_DUTCH(),
        morphuntion::util::LocaleUtils::NETHERLANDS(),
        morphuntion::util::LocaleUtils::POLAND(),
        morphuntion::util::LocaleUtils::BRAZIL(),
        morphuntion::util::LocaleUtils::PORTUGAL(),
        morphuntion::util::LocaleUtils::ROMANIA(),
        morphuntion::util::LocaleUtils::RUSSIA(),
        morphuntion::util::LocaleUtils::SERBIA(),
        morphuntion::util::LocaleUtils::SLOVAKIA(),
        morphuntion::util::LocaleUtils::SWEDEN(),
        morphuntion::util::LocaleUtils::THAILAND(),
        morphuntion::util::LocaleUtils::TURKEY(),
        morphuntion::util::LocaleUtils::UKRAINE(),
        morphuntion::util::LocaleUtils::VIETNAM(),
        morphuntion::util::LocaleUtils::HONGKONG_CHINESE(),
        morphuntion::util::LocaleUtils::CHINA_CANTONESE(),
        morphuntion::util::LocaleUtils::CHINA(),
        morphuntion::util::LocaleUtils::TAIWAN(),
    });
    for (const auto& entry : morphuntion::util::LocaleUtils::getSupportedLocaleMap()) {
        INFO(entry.first);
        CHECK(languages.count(::morphuntion::util::ULocale(entry.first)) == 1);
        for (const auto& locale : entry.second) {
            INFO(locale.getName());
            CHECK(locales.count(locale) == 1);
        }
    }
}

TEST_CASE("LocaleUtilsTest#testInvalid")
{
    LogToString stringLogger;
    std::string locStr(ULOC_FULLNAME_CAPACITY, '@');
    morphuntion::util::ULocale toLong(locStr);
    CHECK(toLong.getName().empty());
    CHECK(toLong.getLanguage().empty());
    CHECK(toLong.getScript().empty());
    CHECK(toLong.getVariant().empty());
    CHECK(toLong.getCountry().empty());
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid locale @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
}
