/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/exception/XMLParseException.hpp>
#include <inflection/message2/MF2Factory.hpp>
#include <inflection/npc.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/util/StringUtils.hpp>

#include "util/TestUtils.hpp"
#include "util/XMLUtils.hpp"
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "unicode/locid.h"
#include "unicode/messageformat2.h"
#include "unicode/messageformat2_function_registry.h"
#include "unicode/messageformat2_formattable.h"

using icu::Locale;
using icu::UnicodeString;
using icu::message2::Formattable;
using icu::message2::data_model::FunctionName;
using icu::message2::MFFunctionRegistry;
using icu::message2::MessageArguments;
using icu::message2::MessageFormatter;


TEST_CASE("MF2Factory#testBasic")
{
    UErrorCode errorCode = U_ZERO_ERROR;
    auto functionName = FunctionName("inflection");
    MFFunctionRegistry::Builder(errorCode)
        .adoptFormatter(
            functionName,
            inflection::message2::MF2Factory::CreateFormatterFactory(),
            errorCode)
        .adoptSelector(
            functionName,
            inflection::message2::MF2Factory::CreateSelectorFactory(),
            errorCode)
        .build();
    REQUIRE(U_SUCCESS(errorCode));
}

TEST_CASE("MF2Factory#testCreateFormatter")
{
    UErrorCode errorCode = U_ZERO_ERROR;
    auto functionName = FunctionName("inflection");
    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
        .adoptFormatter(
            functionName,
            inflection::message2::MF2Factory::CreateFormatterFactory(),
            errorCode)
        .build();
    REQUIRE(U_SUCCESS(errorCode));

    UParseError pe;
    MessageFormatter mf = MessageFormatter::Builder(errorCode)
                .setFunctionRegistry(customRegistry)
                .setLocale(Locale::forLanguageTag("es-MX", errorCode))
                .setPattern("\
Foo {$name :inflection hello=world definiteness=definite \
     number=plural gender=feminine} Bar",
                    pe, errorCode)
                .build(errorCode);
    REQUIRE(U_SUCCESS(errorCode));

    std::map<UnicodeString, Formattable> arguments;
    arguments["name"]= Formattable("gato");

    UnicodeString ret = mf.formatToString(
        MessageArguments(arguments, errorCode), errorCode);
    REQUIRE(U_SUCCESS(errorCode));
    REQUIRE(ret == u"Foo las gatas Bar");
}

TEST_CASE("MF2Factory#testCreateSelector")
{
    UErrorCode errorCode = U_ZERO_ERROR;
    auto functionName = FunctionName("inflection");
    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
        .adoptSelector(
            functionName,
            inflection::message2::MF2Factory::CreateSelectorFactory(),
            errorCode)
        .build();
    REQUIRE(U_SUCCESS(errorCode));

    UParseError pe;
    MessageFormatter mf = MessageFormatter::Builder(errorCode)
                .setFunctionRegistry(customRegistry)
                .setLocale(Locale::forLanguageTag("es-MX", errorCode))
                .setPattern("\
.local $var1 = {$name :inflection select=gender} \
.local $var2 = {$name :inflection select=number} \
.match $var1 $var2\
  masculine 2 {{{$name} is Masculine & 2}} \
  feminine singular {{{$name} is Feminine & Singular}} \
  foo 4 {{{$name} is Foo & 4}} \
  masculine singular {{{$name} is Masculine & Singular}} \
  hello singular {{{$name} is Hello & Singular}} \
  * * {{{$name} is other}}\n",
                    pe, errorCode)
                .build(errorCode);
    REQUIRE(U_SUCCESS(errorCode));

    std::map<UnicodeString, Formattable> arguments;
    arguments["name"]= Formattable("gato");

    UnicodeString ret = mf.formatToString(
        MessageArguments(arguments, errorCode), errorCode);
    REQUIRE(U_SUCCESS(errorCode));
    REQUIRE(ret == u"gato is Masculine & Singular");
}

// Test the case in de
// <test><source number="singular" case="accusative">Bank</source><result indefArticle="eine"/></test>
TEST_CASE("MF2Factory#testSelectorWithoutQuote")
{
    UErrorCode errorCode = U_ZERO_ERROR;
    auto functionName = FunctionName("inflection");
    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
        .adoptSelector(
            functionName,
            inflection::message2::MF2Factory::CreateSelectorFactory(),
            errorCode)
        .build();
    REQUIRE(U_SUCCESS(errorCode));

    UParseError pe;
    MessageFormatter mf = MessageFormatter::Builder(errorCode)
                .setFunctionRegistry(customRegistry)
                .setLocale(Locale::forLanguageTag("de", errorCode))
                .setPattern("\
.local $indefArticle = {$name :inflection number=singular\
                        case=accusative select=indefArticle} \
.match $indefArticle \
eine {{The indefArticle of '{$name}' is 'eine'.}} \
* {{The indefArticle of '{$name}' is something else.}}\n",
                    pe, errorCode)
                .build(errorCode);
    REQUIRE(U_SUCCESS(errorCode));

    std::map<UnicodeString, Formattable> arguments;
    arguments["name"]= Formattable("Bank");

    UnicodeString ret = mf.formatToString(MessageArguments(arguments, errorCode), errorCode);
    REQUIRE(U_SUCCESS(errorCode));
    REQUIRE(ret == u"The indefArticle of 'Bank' is 'eine'.");
}

// Test the case in de
// <test><source number="singular" case="dative">Fundort</source><result withDefArticleInPreposition="im Fundort"/></test>
TEST_CASE("MF2Factory#testSelectorWithQuote")
{
    UErrorCode errorCode = U_ZERO_ERROR;
    auto functionName = FunctionName("inflection");
    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
        .adoptSelector(
            functionName,
            inflection::message2::MF2Factory::CreateSelectorFactory(),
            errorCode)
        .build();
    REQUIRE(U_SUCCESS(errorCode));

    UParseError pe;
    MessageFormatter mf = MessageFormatter::Builder(errorCode)
                .setFunctionRegistry(customRegistry)
                .setLocale(Locale::forLanguageTag("de", errorCode))
                .setPattern("\
.local $withDefArticleInPreposition = {$name \
:inflection number=singular case=dative \
select=withDefArticleInPreposition} \
.match $withDefArticleInPreposition \
|im Fundort| {{The withDefArticleInPreposition of '{$name}' is 'im Fundort'.}} \
* {{The withDefArticleInPreposition of '{$name}' is\
* something else.}}\n",
                    pe, errorCode)
                .build(errorCode);
    REQUIRE(U_SUCCESS(errorCode));

    std::map<UnicodeString, Formattable> arguments;
    arguments["name"]= Formattable("Fundort");

    UnicodeString ret = mf.formatToString(
        MessageArguments(arguments, errorCode), errorCode);
    REQUIRE(U_SUCCESS(errorCode));
    REQUIRE(ret ==
            u"The withDefArticleInPreposition of 'Fundort' is 'im Fundort'.");
}
