/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/exception/ClassCastException.hpp>
#include <inflection/exception/FileNotFoundException.hpp>
#include <inflection/exception/ICUException.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/exception/MissingResourceException.hpp>
#include <inflection/exception/NullPointerException.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/util/Validate.hpp>

TEST_CASE("ExceptionTest#testThrowCatch")
{
    CHECK_THROWS(throw inflection::exception::ClassCastException());
    CHECK_THROWS(throw inflection::exception::ClassCastException(u"arg"));
    CHECK_THROWS(throw inflection::exception::FileNotFoundException());
    CHECK_THROWS(throw inflection::exception::FileNotFoundException(u"arg"));
    CHECK_THROWS(throw inflection::exception::ICUException(U_MEMORY_ALLOCATION_ERROR, u"arg"));
    CHECK_THROWS(throw inflection::exception::IllegalArgumentException());
    CHECK_THROWS(throw inflection::exception::IllegalStateException());
    CHECK_THROWS(throw inflection::exception::IllegalStateException(u"arg"));
    CHECK_THROWS(throw inflection::exception::IncompatibleVersionException());
    CHECK_THROWS(throw inflection::exception::IndexOutOfBoundsException());
    CHECK_THROWS(throw inflection::exception::InvalidConfigurationException());
    CHECK_THROWS(throw inflection::exception::InvalidConfigurationException(u"arg"));
    CHECK_THROWS(throw inflection::exception::MissingResourceException(u"The files are missing", u"inflection::exception::MissingResourceException", u"resource name"));
    CHECK_THROWS(throw inflection::exception::NullPointerException());
    CHECK_THROWS(throw inflection::exception::XMLParseException());
    CHECK_THROWS(throw inflection::exception::XMLParseException(u"arg"));
}

TEST_CASE("ExceptionTest#testMessage")
{
    try {
        throw inflection::exception::NullPointerException(u"Test message");
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::NullPointerException& e) {
        REQUIRE(e.what() == ::std::string("Test message"));
        REQUIRE(e.getMessage() == ::std::u16string(u"Test message"));
    }
}

TEST_CASE("ExceptionTest#testMissingResourceException")
{
    try {
        throw inflection::exception::MissingResourceException(u"Test message", u"CustomFailingClass", u"TheKey");
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::MissingResourceException& e) {
        REQUIRE(e.getClassName() == ::std::u16string(u"CustomFailingClass"));
        REQUIRE(e.getKey() == ::std::u16string(u"TheKey"));
    }
}

TEST_CASE("ExceptionTest#testICUException")
{
    inflection::exception::ICUException::throwOnFailure(U_ZERO_ERROR, u"U_ZERO_ERROR");
    inflection::exception::ICUException::throwOnFailure(U_AMBIGUOUS_ALIAS_WARNING, std::u16string(u"U_AMBIGUOUS_ALIAS_WARNING"));
    try {
        throw inflection::exception::ICUException(U_ILLEGAL_ARGUMENT_ERROR);
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::ICUException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"U_ILLEGAL_ARGUMENT_ERROR"));
    }
    try {
        inflection::exception::ICUException::throwOnFailure(U_ILLEGAL_CHARACTER, u"Please don't do that");
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::ICUException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"U_ILLEGAL_CHARACTER: Please don't do that"));
    }
    try {
        inflection::exception::ICUException::throwOnFailure(U_INVALID_CHAR_FOUND, std::u16string(u"Stop sending me garbage"));
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::ICUException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"U_INVALID_CHAR_FOUND: Stop sending me garbage"));
    }
}

TEST_CASE("ExceptionTest#testStringIndexOutOfBoundsException")
{
    try {
        throw inflection::exception::IndexOutOfBoundsException(u"Please stop going beyond my array boundaries!");
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::IndexOutOfBoundsException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"Please stop going beyond my array boundaries!"));
    }
}

TEST_CASE("ExceptionTest#testValidate")
{
    try {
        inflection::util::Validate::notNull((const void*)nullptr);
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::NullPointerException& e) {
        INFO(e.what());
    }
    try {
        inflection::util::Validate::notNull((const void*)nullptr, u"variable");
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::NullPointerException& e) {
        REQUIRE(e.getMessage() == ::std::u16string(u"Input argument <variable> is null"));
    }
    try {
        inflection::util::Validate::isFalse(true);
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::IllegalArgumentException& e) {
        INFO(e.what());
    }
    try {
        inflection::util::Validate::isTrue(false);
        FAIL("throw did not execute");
    }
    catch (const inflection::exception::IllegalArgumentException& e) {
        INFO(e.what());
    }
}
