/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/LogToString.hpp>
#include <inflection/util/ULocale.hpp>
#include <set>
#include <unicode/uloc.h>
#include <unicode/ulocdata.h>
#include <unicode/unum.h>
#include <unicode/ures.h>

TEST_CASE("LocaleUtilsTest#testCreateValidRegionAwareLanguage")
{
    REQUIRE(::inflection::util::LocaleUtils::SAUDI_ARABIA() == ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(::inflection::util::LocaleUtils::SAUDI_ARABIA(), ::inflection::util::LocaleUtils::US()));
    REQUIRE(::inflection::util::ULocale("ar_TN") == ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(::inflection::util::LocaleUtils::SAUDI_ARABIA(), ::inflection::util::ULocale("ar_TN")));
    // Stay the same script. Only change the region.
    REQUIRE(::inflection::util::ULocale("sr_Cyrl_BA") == ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(::inflection::util::ULocale("sr_Cyrl_RS"), ::inflection::util::ULocale("sr_Latn_BA")));
    REQUIRE(::inflection::util::ULocale("sr_Cyrl_RS") == ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(::inflection::util::ULocale("sr_Cyrl_RS"), ::inflection::util::ULocale("sr_Latn_RS")));
    REQUIRE(::inflection::util::ULocale("es_CO") == ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(::inflection::util::ULocale("es_MX"), ::inflection::util::ULocale("es_CO")));
    REQUIRE(::inflection::util::ULocale("es_MX") == ::inflection::util::LocaleUtils::createValidRegionAwareLanguage(::inflection::util::ULocale("es_CO"), ::inflection::util::ULocale("es_MX")));
}

TEST_CASE("LocaleUtilsTest#testLanguageScriptCompatible")
{
    REQUIRE(::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::US(), ::inflection::util::LocaleUtils::US()));
    REQUIRE(::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::US(), ::inflection::util::LocaleUtils::GREAT_BRITAIN()));
    REQUIRE(!::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::US(), ::inflection::util::LocaleUtils::GERMANY()));
    REQUIRE(::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::CHINA(), ::inflection::util::LocaleUtils::CHINA()));
    REQUIRE(::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::TAIWAN(), ::inflection::util::LocaleUtils::HONGKONG_CHINESE()));
    REQUIRE(::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::CHINA(), ::inflection::util::LocaleUtils::CHINA_CANTONESE()));
    REQUIRE(!::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::CHINA(), ::inflection::util::LocaleUtils::TAIWAN()));
    REQUIRE(!::inflection::util::LocaleUtils::areLanguageScriptCompatible(::inflection::util::LocaleUtils::TAIWAN(), ::inflection::util::LocaleUtils::CHINA_CANTONESE()));
}

TEST_CASE("LocaleUtilsTest#testToString")
{
    REQUIRE(u"zh_CN" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("zh", "CN")));
    REQUIRE(u"en" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("en")));
    REQUIRE(u"zh_CN" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("zh", "CN")));
    REQUIRE(u"he" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("he")));
    REQUIRE(u"he" == ::inflection::util::LocaleUtils::toString(::inflection::util::LocaleUtils::toLocale(u"he")));
    REQUIRE(u"he_IL" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("he", "IL")));
    REQUIRE(u"iw_IL" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("iw", "IL")));
    REQUIRE(u"zh_Hans_CN" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("zh", "hans", "cn")));
    REQUIRE(u"zh-Hans-CN" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("zh", "hans", "cn"), u"-"));
    REQUIRE(u"en" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("en"), u"-"));
    REQUIRE(u"en/US" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("en", "us"), u"/"));
    REQUIRE("vi_VN@sd=vnct" == ::inflection::util::ULocale("vi", "VN", "sd=vnct").getName());
    REQUIRE("vi_VN@sd=vnct" == ::inflection::util::ULocale("vi", "VN", "", "sd=vnct").getName());
    REQUIRE(u"vi_VN_sd=vnct" == ::inflection::util::LocaleUtils::toString(::inflection::util::ULocale("vi", "VN", "", "sd=vnct")));
}

TEST_CASE("LocaleUtilsTest#hashTest")
{
    inflection::util::ULocale locale1 = inflection::util::LocaleUtils::ARABIC();
    inflection::util::ULocale locale2 = inflection::util::LocaleUtils::GERMAN();
    inflection::util::ULocale locale3 = inflection::util::LocaleUtils::HINDI();

    std::set<inflection::util::ULocale> s;
    s.insert(locale2);
    s.insert(locale1);
    s.insert(locale3);

    auto it = s.begin();

    REQUIRE(locale1 == *it);
    REQUIRE(locale2 == *(++it));
    REQUIRE(locale3 == *(++it));
}

TEST_CASE("LocaleUtilsTest#testCoverage")
{
    // Ensure that the constants and the dynamic list sync up.
    ::std::set<::inflection::util::ULocale> languages({
        inflection::util::LocaleUtils::ARABIC(),
        inflection::util::LocaleUtils::BANGLA(),
        inflection::util::LocaleUtils::BULGARIAN(),
        inflection::util::LocaleUtils::CATALAN(),
        inflection::util::LocaleUtils::CZECH(),
        inflection::util::LocaleUtils::DANISH(),
        inflection::util::LocaleUtils::GERMAN(),
        inflection::util::LocaleUtils::GREEK(),
        inflection::util::LocaleUtils::ENGLISH(),
        inflection::util::LocaleUtils::SPANISH(),
        inflection::util::LocaleUtils::ESTONIAN(),
        inflection::util::LocaleUtils::FINNISH(),
        inflection::util::LocaleUtils::FRENCH(),
        inflection::util::LocaleUtils::GUJARATI(),
        inflection::util::LocaleUtils::HEBREW(),
        inflection::util::LocaleUtils::HINDI(),
        inflection::util::LocaleUtils::CROATIAN(),
        inflection::util::LocaleUtils::HUNGARIAN(),
        inflection::util::LocaleUtils::INDONESIAN(),
        inflection::util::LocaleUtils::ICELANDIC(),
        inflection::util::LocaleUtils::ITALIAN(),
        inflection::util::LocaleUtils::JAPANESE(),
        inflection::util::LocaleUtils::KANNADA(),
        inflection::util::LocaleUtils::KAZAKH(),
        inflection::util::LocaleUtils::KOREAN(),
        inflection::util::LocaleUtils::LITHUANIAN(),
        inflection::util::LocaleUtils::MALAY(),
        inflection::util::LocaleUtils::MALAYALAM(),
        inflection::util::LocaleUtils::MARATHI(),
        inflection::util::LocaleUtils::NORWEGIAN(),
        inflection::util::LocaleUtils::DUTCH(),
        inflection::util::LocaleUtils::ODIA(),
        inflection::util::LocaleUtils::POLISH(),
        inflection::util::LocaleUtils::PORTUGUESE(),
        inflection::util::LocaleUtils::PUNJABI(),
        inflection::util::LocaleUtils::ROMANIAN(),
        inflection::util::LocaleUtils::RUSSIAN(),
        inflection::util::LocaleUtils::SERBIAN(),
        inflection::util::LocaleUtils::SLOVAK(),
        inflection::util::LocaleUtils::SWEDISH(),
        inflection::util::LocaleUtils::TAMIL(),
        inflection::util::LocaleUtils::TELUGU(),
        inflection::util::LocaleUtils::THAI(),
        inflection::util::LocaleUtils::TURKISH(),
        inflection::util::LocaleUtils::UKRAINIAN(),
        inflection::util::LocaleUtils::URDU(),
        inflection::util::LocaleUtils::VIETNAMESE(),
        inflection::util::LocaleUtils::CANTONESE(),
        inflection::util::LocaleUtils::CHINESE(),
    });
    ::std::set<::inflection::util::ULocale> locales({
        inflection::util::LocaleUtils::SAUDI_ARABIA(),
        inflection::util::LocaleUtils::INDIA_BANGLA(),
        inflection::util::LocaleUtils::BULGARIA(),
        inflection::util::LocaleUtils::SPAIN_CATALAN(),
        inflection::util::LocaleUtils::CZECHIA(),
        inflection::util::LocaleUtils::DENMARK(),
        inflection::util::LocaleUtils::GERMANY(),
        inflection::util::LocaleUtils::AUSTRIA(),
        inflection::util::LocaleUtils::SWITZERLAND_GERMAN(),
        inflection::util::LocaleUtils::GREECE(),
        inflection::util::LocaleUtils::AUSTRALIA_ENGLISH(),
        inflection::util::LocaleUtils::CANADIAN_ENGLISH(),
        inflection::util::LocaleUtils::GREAT_BRITAIN(),
        inflection::util::LocaleUtils::IRISH_ENGLISH(),
        inflection::util::LocaleUtils::INDIA_ENGLISH(),
        inflection::util::LocaleUtils::NEW_ZEALAND_ENGLISH(),
        inflection::util::LocaleUtils::SINGAPOREAN_ENGLISH(),
        inflection::util::LocaleUtils::US(),
        inflection::util::LocaleUtils::SOUTH_AFRICAN_ENGLISH(),
        inflection::util::LocaleUtils::CHILEAN_SPANISH(),
        inflection::util::LocaleUtils::SPAIN_SPANISH(),
        inflection::util::LocaleUtils::MEXICO_SPANISH(),
        inflection::util::LocaleUtils::US_SPANISH(),
        inflection::util::LocaleUtils::ESTONIA(),
        inflection::util::LocaleUtils::FINLAND(),
        inflection::util::LocaleUtils::BELGIUM_FRENCH(),
        inflection::util::LocaleUtils::CANADIAN_FRENCH(),
        inflection::util::LocaleUtils::FRANCE(),
        inflection::util::LocaleUtils::SWITZERLAND_FRENCH(),
        inflection::util::LocaleUtils::INDIA_GUJARATI(),
        inflection::util::LocaleUtils::INDIA_HINDI(),
        inflection::util::LocaleUtils::INDIA_MALAYALAM(),
        inflection::util::LocaleUtils::CROATIA(),
        inflection::util::LocaleUtils::ISRAEL(),
        inflection::util::LocaleUtils::HUNGARY(),
        inflection::util::LocaleUtils::INDONESIA(),
        inflection::util::LocaleUtils::ICELAND(),
        inflection::util::LocaleUtils::SWITZERLAND_ITALIAN(),
        inflection::util::LocaleUtils::ITALY(),
        inflection::util::LocaleUtils::JAPAN(),
        inflection::util::LocaleUtils::INDIA_KANNADA(),
        inflection::util::LocaleUtils::KAZAKHSTAN(),
        inflection::util::LocaleUtils::KOREA(),
        inflection::util::LocaleUtils::LITHUANIA(),
        inflection::util::LocaleUtils::INDIA_MARATHI(),
        inflection::util::LocaleUtils::MALAYSIA(),
        inflection::util::LocaleUtils::NORWAY(),
        inflection::util::LocaleUtils::BELGIUM_DUTCH(),
        inflection::util::LocaleUtils::NETHERLANDS(),
        inflection::util::LocaleUtils::INDIA_ODIA(),
        inflection::util::LocaleUtils::INDIA_PUNJABI(),
        inflection::util::LocaleUtils::POLAND(),
        inflection::util::LocaleUtils::BRAZIL(),
        inflection::util::LocaleUtils::PORTUGAL(),
        inflection::util::LocaleUtils::ROMANIA(),
        inflection::util::LocaleUtils::RUSSIA(),
        inflection::util::LocaleUtils::SERBIA(),
        inflection::util::LocaleUtils::SLOVAKIA(),
        inflection::util::LocaleUtils::SWEDEN(),
        inflection::util::LocaleUtils::INDIA_TAMIL(),
        inflection::util::LocaleUtils::INDIA_TELUGU(),
        inflection::util::LocaleUtils::THAILAND(),
        inflection::util::LocaleUtils::TURKEY(),
        inflection::util::LocaleUtils::UKRAINE(),
        inflection::util::LocaleUtils::PAKISTAN_URDU(),
        inflection::util::LocaleUtils::VIETNAM(),
        inflection::util::LocaleUtils::HONGKONG_CHINESE(),
        inflection::util::LocaleUtils::CHINA_CANTONESE(),
        inflection::util::LocaleUtils::CHINA(),
        inflection::util::LocaleUtils::TAIWAN(),
    });
    for (const auto& entry : inflection::util::LocaleUtils::getSupportedLocaleMap()) {
        INFO(entry.first);
        CHECK(languages.count(::inflection::util::ULocale(entry.first)) == 1);
        for (const auto& locale : entry.second) {
            INFO(locale.getName());
            CHECK(locales.count(locale) == 1);
        }
    }
}

// Verifies that every supported locale has its required ICU data bundles present,
// acting as a guardrail when ICU is built with icu_filters.json (while also passing
// against a full, unfiltered ICU build).
TEST_CASE("LocaleUtilsTest#testMinimalICUData")
{
    for (const auto& [languageStr, locales] : inflection::util::LocaleUtils::getSupportedLocaleMap()) {
        INFO(languageStr);
        ::inflection::util::ULocale language(languageStr);
        for (const auto& locale : locales) {
            INFO(locale.getName());

            // 1. Verify that the exact locale bundle exists in locales_tree without falling
            // back to a parent or default locale (status == U_ZERO_ERROR).
            // We query ures_open(nullptr, ...) directly rather than checking status on
            // ulocdata_open, because ulocdata_open also opens U_ICUDATA_LANG (lang_tree);
            // on unfiltered ICU builds where lang_tree is present, regional locales (e.g. en_US)
            // fall back to their parent language in lang_tree and set U_USING_FALLBACK_WARNING.
            auto status = U_ZERO_ERROR;
            auto bundle = ures_open(nullptr, locale.getName().c_str(), &status);
            REQUIRE(U_SUCCESS(status));
            CHECK(status == U_ZERO_ERROR);
            ures_close(bundle);

            // 2. Verify that ExemplarCharacters can be loaded and is non-empty.
            // Since root defines an empty ExemplarCharacters{"[]"}, a non-empty set confirms
            // that the locale (or its parent language bundle) provides ExemplarCharacters.
            // Note: we intentionally do not call ulocdata_setNoSubstitute(localeData, true)
            // because ICU's uresbund.cpp sets U_USING_DEFAULT_WARNING whenever a child locale
            // (e.g. en_US) inherits a resource from a parent bundle (e.g. en) whose name
            // happens to match uloc_getDefault() (e.g. when LC_ALL=en).
            status = U_ZERO_ERROR;
            auto localeData = ulocdata_open(locale.getName().c_str(), &status);
            REQUIRE(U_SUCCESS(status));
            icu4cxx::UnicodeSet exemplarSet;
            status = U_ZERO_ERROR;
            ulocdata_getExemplarSet(localeData, exemplarSet.wrappee_, USET_CASE_INSENSITIVE, ULOCDATA_ES_STANDARD, &status);
            CHECK(U_SUCCESS(status));
            CHECK(!exemplarSet.isEmpty());
            ulocdata_close(localeData);

            // 3. Verify that RBNF spellout rules exist in rbnf_tree for this locale's language
            // and did not fall back to the system default locale.
            // We check ULOC_VALID_LOCALE rather than status != U_USING_DEFAULT_WARNING because
            // RuleBasedNumberFormat::init constructs DecimalFormatSymbols, which looks up
            // currencySpacing in curr_tree and sets U_USING_DEFAULT_WARNING when curr_tree
            // is filtered to root only. Note that zh_HK aliases to "yue" in ICU's rbnf_tree.
            status = U_ZERO_ERROR;
            auto rbnf = unum_open(UNUM_SPELLOUT, nullptr, 0, locale.getName().c_str(), nullptr, &status);
            REQUIRE(U_SUCCESS(status));
            auto validRbnfLocale = unum_getLocaleByType(rbnf, ULOC_VALID_LOCALE, &status);
            REQUIRE(U_SUCCESS(status));
            CHECK((::inflection::util::ULocale(validRbnfLocale).getLanguage() == language.getLanguage()
                || (locale == inflection::util::LocaleUtils::HONGKONG_CHINESE() && ::std::string_view(validRbnfLocale) == "yue")));
            unum_close(rbnf);
        }
    }
}

TEST_CASE("LocaleUtilsTest#testInvalid")
{
    LogToString stringLogger;
    std::string locStr(ULOC_FULLNAME_CAPACITY, '@');
    inflection::util::ULocale toLong(locStr);
    CHECK(toLong.getName().empty());
    CHECK(toLong.getLanguage().empty());
    CHECK(toLong.getScript().empty());
    CHECK(toLong.getVariant().empty());
    CHECK(toLong.getCountry().empty());
    REQUIRE(stringLogger.logLines.size() == 1);
    CHECK(stringLogger.logLines[0] == u"[ERROR] Invalid locale @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
}
