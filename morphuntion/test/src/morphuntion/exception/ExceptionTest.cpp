/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/exception/FileNotFoundException.hpp>
#include <morphuntion/exception/ICUException.hpp>
#include <morphuntion/exception/IllegalArgumentException.hpp>
#include <morphuntion/exception/IllegalStateException.hpp>
#include <morphuntion/exception/IncompatibleVersionException.hpp>
#include <morphuntion/exception/InvalidConfigurationException.hpp>
#include <morphuntion/exception/MissingResourceException.hpp>
#include <morphuntion/exception/NullPointerException.hpp>
#include <morphuntion/exception/StringIndexOutOfBoundsException.hpp>
#include <morphuntion/exception/XMLParseException.hpp>
#include <morphuntion/util/Validate.hpp>

TEST_CASE("ExceptionTest#testThrowCatch")
{
    CHECK_THROWS(throw morphuntion::exception::ClassCastException());
    CHECK_THROWS(throw morphuntion::exception::ClassCastException(u"arg"));
    CHECK_THROWS(throw morphuntion::exception::FileNotFoundException());
    CHECK_THROWS(throw morphuntion::exception::FileNotFoundException(u"arg"));
    CHECK_THROWS(throw morphuntion::exception::ICUException(U_MEMORY_ALLOCATION_ERROR, u"arg"));
    CHECK_THROWS(throw morphuntion::exception::IllegalArgumentException());
    CHECK_THROWS(throw morphuntion::exception::IllegalStateException());
    CHECK_THROWS(throw morphuntion::exception::IllegalStateException(u"arg"));
    CHECK_THROWS(throw morphuntion::exception::IncompatibleVersionException());
    CHECK_THROWS(throw morphuntion::exception::IndexOutOfBoundsException());
    CHECK_THROWS(throw morphuntion::exception::InvalidConfigurationException());
    CHECK_THROWS(throw morphuntion::exception::InvalidConfigurationException(u"arg"));
    CHECK_THROWS(throw morphuntion::exception::MissingResourceException(u"The files are missing", u"morphuntion::exception::MissingResourceException", u"resource name"));
    CHECK_THROWS(throw morphuntion::exception::NullPointerException());
    CHECK_THROWS(throw morphuntion::exception::StringIndexOutOfBoundsException());
    CHECK_THROWS(throw morphuntion::exception::XMLParseException());
    CHECK_THROWS(throw morphuntion::exception::XMLParseException(u"arg"));
}

TEST_CASE("ExceptionTest#testMessage")
{
    try {
        throw morphuntion::exception::NullPointerException(u"Test message");
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::NullPointerException& e) {
        REQUIRE(e.what() == ::std::string("Test message"));
        REQUIRE(e.getMessage() == ::std::u16string(u"Test message"));
    }
}

TEST_CASE("ExceptionTest#testMissingResourceException")
{
    try {
        throw morphuntion::exception::MissingResourceException(u"Test message", u"CustomFailingClass", u"TheKey");
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::MissingResourceException& e) {
        REQUIRE(e.getClassName() == ::std::u16string(u"CustomFailingClass"));
        REQUIRE(e.getKey() == ::std::u16string(u"TheKey"));
    }
}

TEST_CASE("ExceptionTest#testICUException")
{
    morphuntion::exception::ICUException::throwOnFailure(U_ZERO_ERROR, u"U_ZERO_ERROR");
    morphuntion::exception::ICUException::throwOnFailure(U_AMBIGUOUS_ALIAS_WARNING, std::u16string(u"U_AMBIGUOUS_ALIAS_WARNING"));
    try {
        throw morphuntion::exception::ICUException(U_ILLEGAL_ARGUMENT_ERROR);
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::ICUException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"U_ILLEGAL_ARGUMENT_ERROR"));
    }
    try {
        morphuntion::exception::ICUException::throwOnFailure(U_ILLEGAL_CHARACTER, u"Please don't do that");
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::ICUException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"U_ILLEGAL_CHARACTER: Please don't do that"));
    }
    try {
        morphuntion::exception::ICUException::throwOnFailure(U_INVALID_CHAR_FOUND, std::u16string(u"Stop sending me garbage"));
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::ICUException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"U_INVALID_CHAR_FOUND: Stop sending me garbage"));
    }
}

TEST_CASE("ExceptionTest#testStringIndexOutOfBoundsException")
{
    try {
        throw morphuntion::exception::StringIndexOutOfBoundsException(u"Please stop going beyond my array boundaries!");
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::StringIndexOutOfBoundsException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"Please stop going beyond my array boundaries!"));
    }
}

TEST_CASE("ExceptionTest#testValidate")
{
    try {
        morphuntion::util::Validate::notNull((const void*)nullptr);
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::NullPointerException& e) {
        INFO(e.what());
    }
    try {
        morphuntion::util::Validate::notNull((const void*)nullptr, u"variable");
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::NullPointerException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"Input argument <variable> is null"));
    }
    try {
        morphuntion::util::Validate::isFalse(true);
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::IllegalArgumentException& e) {
        INFO(e.what());
    }
    try {
        morphuntion::util::Validate::isTrue(false);
        FAIL("throw did not execute");
    }
    catch (const morphuntion::exception::IllegalArgumentException& e) {
        INFO(e.what());
    }
}
