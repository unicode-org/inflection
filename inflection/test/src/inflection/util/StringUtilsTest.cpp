/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/util/StringUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <math.h>

TEST_CASE("StringUtilsTest#codePointAt")
{
    REQUIRE(inflection::util::StringViewUtils::codePointAt(u"123", 0) == u'1');
    REQUIRE(inflection::util::StringViewUtils::codePointAt(u"123", 2) == u'3');
    REQUIRE_THROWS_AS(inflection::util::StringViewUtils::codePointAt(u"123", -1), ::inflection::exception::IndexOutOfBoundsException);
    REQUIRE(inflection::util::StringViewUtils::codePointAt(u"\U00010000", 0) == 0x10000);
    REQUIRE(inflection::util::StringViewUtils::codePointAt(u"\U00010000", 1) == 0x10000);
    REQUIRE_THROWS_AS(inflection::util::StringViewUtils::codePointAt(u"\U00010000", 2), ::inflection::exception::IndexOutOfBoundsException);
}

TEST_CASE("StringUtilsTest#convert")
{
    REQUIRE(inflection::util::StringUtils::to_string(u"123") == "123");
    REQUIRE(inflection::util::StringUtils::to_string(u"1😀の") == "1😀の");
    REQUIRE(inflection::util::StringUtils::to_u16string("123") == u"123");
    REQUIRE(inflection::util::StringUtils::to_u16string("1😀の") == u"1😀の");
    REQUIRE(inflection::util::StringUtils::to_u16string(1.2) == u"1.200000");
    REQUIRE(inflection::util::StringUtils::to_u16string(3.1415926535) == u"3.141593");
    REQUIRE(inflection::util::StringUtils::to_u16string(NAN) == u"nan");
    REQUIRE(inflection::util::StringUtils::to_u16string(INFINITY) == u"inf");
}

TEST_CASE("StringUtilsTest#reverse")
{
    ::std::u16string string;
    string = u"123";
    REQUIRE(*inflection::util::StringUtils::reverse(&string) == u"321");
    string = u"";
    REQUIRE(*inflection::util::StringUtils::reverse(&string) == u"");
    string = u"A";
    REQUIRE(*inflection::util::StringUtils::reverse(&string) == u"A");
    string = u"1😀の";
    REQUIRE(*inflection::util::StringUtils::reverse(&string) == u"の😀1");
    string = u"12😀の";
    REQUIRE(*inflection::util::StringUtils::reverse(&string) == u"の😀21");
}

TEST_CASE("StringUtilsTest#isAllUpperCase")
{
    REQUIRE_FALSE(inflection::util::StringViewUtils::isAllUpperCase(u"abc"));
    REQUIRE_FALSE(inflection::util::StringViewUtils::isAllUpperCase(u"Abc"));
    REQUIRE_FALSE(inflection::util::StringViewUtils::isAllUpperCase(u"aBC"));
    REQUIRE(inflection::util::StringViewUtils::isAllUpperCase(u"ABC"));
}

TEST_CASE("StringUtilsTest#uppercase")
{
    ::std::u16string string;
    REQUIRE(*inflection::util::StringViewUtils::uppercase(&string, u"abc", inflection::util::LocaleUtils::ENGLISH()) == u"ABC");
    REQUIRE(*inflection::util::StringViewUtils::uppercase(&string, u"ABC", inflection::util::LocaleUtils::ENGLISH()) == u"ABC");
    REQUIRE(*inflection::util::StringViewUtils::uppercase(&string, u"の😀1", inflection::util::LocaleUtils::ENGLISH()) == u"の😀1");
    REQUIRE(*inflection::util::StringViewUtils::uppercase(&string, u"SsßiIΐΣσς", inflection::util::LocaleUtils::ENGLISH()) == u"SSSSIIΪ́ΣΣΣ"); // It grew!
    REQUIRE(*inflection::util::StringViewUtils::uppercase(&string, u"SsßiıΐΣσς", inflection::util::LocaleUtils::TURKISH()) == u"SSSSİIΪ́ΣΣΣ"); // It grew and is locale aware!
    REQUIRE(*inflection::util::StringViewUtils::uppercase(&string, u"ẘ", inflection::util::LocaleUtils::ENGLISH()) == u"W\u030A"); // It grew!
}

TEST_CASE("StringUtilsTest#lowercase")
{
    ::std::u16string string;
    REQUIRE(*inflection::util::StringViewUtils::lowercase(&string, u"ABC", inflection::util::LocaleUtils::ENGLISH()) == u"abc");
    REQUIRE(*inflection::util::StringViewUtils::lowercase(&string, u"abc", inflection::util::LocaleUtils::ENGLISH()) == u"abc");
    REQUIRE(*inflection::util::StringViewUtils::lowercase(&string, u"の😀1", inflection::util::LocaleUtils::ENGLISH()) == u"の😀1");
    REQUIRE(*inflection::util::StringViewUtils::lowercase(&string, u"SsßiIÌΐ𐐝Σσς", inflection::util::LocaleUtils::ENGLISH()) == u"ssßiiìΐ𐑅σσς");
    REQUIRE(*inflection::util::StringViewUtils::lowercase(&string, u"SsßİIÌΐ𐐝Σσς", inflection::util::LocaleUtils::TURKISH()) == u"ssßiıìΐ𐑅σσς");
    REQUIRE(*inflection::util::StringViewUtils::lowercase(&string, u"I\u0307", inflection::util::LocaleUtils::TURKISH()) == u"i"); // It shrank and is locale aware!
    REQUIRE(*inflection::util::StringViewUtils::lowercase(&string, u"Ĩ", inflection::util::ULocale("lt")) == u"i̇̃"); // It grew and is locale aware!
}
