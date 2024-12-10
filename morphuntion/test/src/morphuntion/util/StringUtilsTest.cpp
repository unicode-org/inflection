/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/exception/StringIndexOutOfBoundsException.hpp>
#include <math.h>

TEST_CASE("StringUtilsTest#codePointAt")
{
    REQUIRE(morphuntion::util::StringViewUtils::codePointAt(u"123", 0) == u'1');
    REQUIRE(morphuntion::util::StringViewUtils::codePointAt(u"123", 2) == u'3');
    REQUIRE_THROWS_AS(morphuntion::util::StringViewUtils::codePointAt(u"123", -1), ::morphuntion::exception::StringIndexOutOfBoundsException);
    REQUIRE(morphuntion::util::StringViewUtils::codePointAt(u"\U00010000", 0) == 0x10000);
    REQUIRE(morphuntion::util::StringViewUtils::codePointAt(u"\U00010000", 1) == 0x10000);
    REQUIRE_THROWS_AS(morphuntion::util::StringViewUtils::codePointAt(u"\U00010000", 2), ::morphuntion::exception::StringIndexOutOfBoundsException);
}

TEST_CASE("StringUtilsTest#startsWith")
{
    REQUIRE(morphuntion::util::StringViewUtils::startsWith(u"123", u""));
    REQUIRE(!morphuntion::util::StringViewUtils::startsWith(u"123", u"a"));
    REQUIRE(morphuntion::util::StringViewUtils::startsWith(u"123", u"1"));
    REQUIRE(morphuntion::util::StringViewUtils::startsWith(u"123", u"123"));
    REQUIRE(!morphuntion::util::StringViewUtils::startsWith(u"123", u"1234"));
}

TEST_CASE("StringUtilsTest#endsWith")
{
    REQUIRE(morphuntion::util::StringViewUtils::endsWith(u"123", u""));
    REQUIRE(!morphuntion::util::StringViewUtils::endsWith(u"123", u"a"));
    REQUIRE(morphuntion::util::StringViewUtils::endsWith(u"123", u"3"));
    REQUIRE(morphuntion::util::StringViewUtils::endsWith(u"123", u"123"));
    REQUIRE(!morphuntion::util::StringViewUtils::endsWith(u"123", u"0123"));
}

TEST_CASE("StringUtilsTest#convert")
{
    REQUIRE(morphuntion::util::StringUtils::to_string(u"123") == "123");
    REQUIRE(morphuntion::util::StringUtils::to_string(u"1😀の") == "1😀の");
    REQUIRE(morphuntion::util::StringUtils::to_u16string("123") == u"123");
    REQUIRE(morphuntion::util::StringUtils::to_u16string("1😀の") == u"1😀の");
    REQUIRE(morphuntion::util::StringUtils::to_u16string(1.2) == u"1.200000");
    REQUIRE(morphuntion::util::StringUtils::to_u16string(3.1415926535) == u"3.141593");
    REQUIRE(morphuntion::util::StringUtils::to_u16string(NAN) == u"nan");
    REQUIRE(morphuntion::util::StringUtils::to_u16string(INFINITY) == u"inf");
}

TEST_CASE("StringUtilsTest#reverse")
{
    ::std::u16string string;
    string = u"123";
    REQUIRE(*morphuntion::util::StringUtils::reverse(&string) == u"321");
    string = u"";
    REQUIRE(*morphuntion::util::StringUtils::reverse(&string) == u"");
    string = u"A";
    REQUIRE(*morphuntion::util::StringUtils::reverse(&string) == u"A");
    string = u"1😀の";
    REQUIRE(*morphuntion::util::StringUtils::reverse(&string) == u"の😀1");
    string = u"12😀の";
    REQUIRE(*morphuntion::util::StringUtils::reverse(&string) == u"の😀21");
}

TEST_CASE("StringUtilsTest#isAllUpperCase")
{
    REQUIRE_FALSE(morphuntion::util::StringViewUtils::isAllUpperCase(u"abc"));
    REQUIRE_FALSE(morphuntion::util::StringViewUtils::isAllUpperCase(u"Abc"));
    REQUIRE_FALSE(morphuntion::util::StringViewUtils::isAllUpperCase(u"aBC"));
    REQUIRE(morphuntion::util::StringViewUtils::isAllUpperCase(u"ABC"));
}

TEST_CASE("StringUtilsTest#uppercase")
{
    ::std::u16string string;
    REQUIRE(*morphuntion::util::StringViewUtils::uppercase(&string, u"abc", morphuntion::util::LocaleUtils::ENGLISH()) == u"ABC");
    REQUIRE(*morphuntion::util::StringViewUtils::uppercase(&string, u"ABC", morphuntion::util::LocaleUtils::ENGLISH()) == u"ABC");
    REQUIRE(*morphuntion::util::StringViewUtils::uppercase(&string, u"の😀1", morphuntion::util::LocaleUtils::ENGLISH()) == u"の😀1");
    REQUIRE(*morphuntion::util::StringViewUtils::uppercase(&string, u"SsßiIΐΣσς", morphuntion::util::LocaleUtils::ENGLISH()) == u"SSSSIIΪ́ΣΣΣ"); // It grew!
    REQUIRE(*morphuntion::util::StringViewUtils::uppercase(&string, u"SsßiıΐΣσς", morphuntion::util::LocaleUtils::TURKISH()) == u"SSSSİIΪ́ΣΣΣ"); // It grew and is locale aware!
    REQUIRE(*morphuntion::util::StringViewUtils::uppercase(&string, u"ẘ", morphuntion::util::LocaleUtils::ENGLISH()) == u"W\u030A"); // It grew!
}

TEST_CASE("StringUtilsTest#lowercase")
{
    ::std::u16string string;
    REQUIRE(*morphuntion::util::StringViewUtils::lowercase(&string, u"ABC", morphuntion::util::LocaleUtils::ENGLISH()) == u"abc");
    REQUIRE(*morphuntion::util::StringViewUtils::lowercase(&string, u"abc", morphuntion::util::LocaleUtils::ENGLISH()) == u"abc");
    REQUIRE(*morphuntion::util::StringViewUtils::lowercase(&string, u"の😀1", morphuntion::util::LocaleUtils::ENGLISH()) == u"の😀1");
    REQUIRE(*morphuntion::util::StringViewUtils::lowercase(&string, u"SsßiIÌΐ𐐝Σσς", morphuntion::util::LocaleUtils::ENGLISH()) == u"ssßiiìΐ𐑅σσς");
    REQUIRE(*morphuntion::util::StringViewUtils::lowercase(&string, u"SsßİIÌΐ𐐝Σσς", morphuntion::util::LocaleUtils::TURKISH()) == u"ssßiıìΐ𐑅σσς");
    REQUIRE(*morphuntion::util::StringViewUtils::lowercase(&string, u"I\u0307", morphuntion::util::LocaleUtils::TURKISH()) == u"i"); // It shrank and is locale aware!
    REQUIRE(*morphuntion::util::StringViewUtils::lowercase(&string, u"Ĩ", morphuntion::util::ULocale("lt")) == u"i̇̃"); // It grew and is locale aware!
}
