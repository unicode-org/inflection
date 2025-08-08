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

static std::u16string getU16String(xmlNodePtr node) {
    xmlNodePtr textNode = xmlFirstElementChild(node);
    if (textNode == nullptr) {
        std::u16string printStr;
        XMLUtils::getContent(&printStr, node);
        return printStr;
    }
    REQUIRE(xmlStrEqual(textNode->name, (const xmlChar *) "text"));
    return XMLUtils::getRequiredAttribute(textNode, "print");
}

std::u16string BuildSelectorPatternFromConstraints(
    const std::map<std::u16string, std::u16string>& constraints,
    const std::u16string& feature,
    const std::u16string& match,
    const std::u16string& correctResult) {
    std::u16string result(u".local $feature = {$source :inflection select=");
    result += feature;
    for (const auto& [key, value] : constraints) {
        result += u" " + key + u"=" + value;
    }
    result += u"} ";
    result += u".match $feature ";
    result += u"|" + match + u"| {{" + correctResult + u"}} ";
    result += u"* {{Wrong}}";
    return result;
}

std::u16string BuildFormatterPatternFromConstraints(
    const std::map<std::u16string, std::u16string>& constraints) {
    std::u16string result(u"{$source :inflection");
    for (const auto& [key, value] : constraints) {
        result += u" " + key + u"=" + value;
    }
    result += u"}";

    return result;
}

TEST_CASE("MF2Factory#testFormaterFromDataDriven")
{
    const auto &testLocales = TestUtils::getTestLocaleMap();
    std::string resourcePath(TestUtils::getTestResourcePath() + "dialog/inflection/");
    auto files(TestUtils::listDirectoryContents(resourcePath));
    std::sort(files.begin(), files.end());
    int32_t numFiles = 0;

    UErrorCode errorCode = U_ZERO_ERROR;
    auto functionName = FunctionName("inflection");
    auto customRegistry = MFFunctionRegistry::Builder(errorCode)
        .adoptFormatter(functionName, inflection::message2::MF2Factory::CreateFormatterFactory(), errorCode)
        .adoptSelector(functionName, inflection::message2::MF2Factory::CreateSelectorFactory(), errorCode)
        .build();
    REQUIRE(U_SUCCESS(errorCode));

    MessageFormatter::Builder builder(errorCode);
    builder.setFunctionRegistry(customRegistry);

    std::map<UnicodeString, Formattable> arguments;
    for (const auto& file : files) {
        ::std::u16string resourceFile;
        ::inflection::util::StringUtils::convert(&resourceFile, resourcePath+file);
        ::inflection::util::MemoryMappedFile mMapFile(resourceFile);
        xmlDocPtr xmlDoc = xmlParseMemory(mMapFile.getData(), (int)mMapFile.getSize());
        if (xmlDoc == nullptr) {
            throw ::inflection::exception::IOException(resourceFile);
        }
        xmlNodePtr tokenizerTest = xmlDocGetRootElement(xmlDoc);
        if (tokenizerTest == nullptr) {
            throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <inflectionTest>, got <null>");
        }

        ::std::string localeStr = file.substr(0, file.length() - ::std::string(".xml").length());
        ::inflection::util::ULocale locale(localeStr);
        if (testLocales.find(locale.getLanguage()) == testLocales.end()) {
            continue;
        }
        printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
        printf("<!--\n");
        printf(" Copyright 2025 Unicode Incorporated and others. All rights reserved.\n");
        printf("-->\n");
        printf("<messagetest locale=\"%s\">\n", localeStr.c_str());
        // set the new locale to builder
        builder.setLocale(Locale::forLanguageTag(locale.getLanguage(), errorCode));
        REQUIRE(U_SUCCESS(errorCode));

        numFiles++;

        int32_t numTests = 0;
        for (xmlNodePtr currTestNode = tokenizerTest->children; currTestNode != nullptr; currTestNode = currTestNode->next) {
            if (currTestNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            REQUIRE(xmlStrEqual(currTestNode->name, (const xmlChar *) "test") != 0);
            xmlNodePtr curTestChild = xmlFirstElementChild(currTestNode);
            xmlNodePtr sourceNode = curTestChild;
            REQUIRE(xmlStrEqual(sourceNode->name, (const xmlChar *) "source") != 0);
            xmlNodePtr resultNode = xmlNextElementSibling(sourceNode);
            REQUIRE(xmlStrEqual(resultNode->name, (const xmlChar *) "result") != 0);
            auto sourceConstraints(XMLUtils::getAttributes(sourceNode));
            auto resultConstraints(XMLUtils::getAttributes(resultNode));
            std::u16string patternString = BuildFormatterPatternFromConstraints(sourceConstraints);
            auto resultString = getU16String(resultNode);
            // Test formatter if result is empty
            if (resultString.length() != 0) {
                patternString = BuildFormatterPatternFromConstraints(sourceConstraints);
            } else {
                if(resultConstraints.size() != 1) {
                    // skip empty result with no attribute.
                    continue;
                }
                auto iter = resultConstraints.begin();
                if (iter->first == u"exists") {
                    // skip exists test
                    continue;
                }
                resultString = u"correct answer";
                patternString = BuildSelectorPatternFromConstraints(sourceConstraints,
                                                                    iter->first,
                                                                    iter->second,
                                                                    resultString);
            }
            arguments["source"]= Formattable(getU16String(sourceNode));

            UnicodeString p(patternString);
            std::string p8;
            printf("    <test>\n");
            printf("        <params>\n");
            printf("            <param name=\"source\" type=\"String\"><value>%s</value></param>\n",
                   arguments["source"]
                   .getString(errorCode)
                   .toUTF8String<std::string>(p8).c_str());
            printf("        </params>\n");
            p8.clear();
            printf("        <source>%s</source>\n", p.toUTF8String<std::string>(p8).c_str());
            UnicodeString r(resultString);
            p8.clear();
            printf("        <print>%s</print>\n", r.toUTF8String<std::string>(p8).c_str());
            printf("    </test>\n");
            UParseError pe;
            auto actualResult = builder.setPattern(patternString, pe, errorCode)
                .build(errorCode)
                .formatToString(MessageArguments(arguments, errorCode), errorCode);
            REQUIRE(U_SUCCESS(errorCode));

            REQUIRE(actualResult == resultString);
            numTests++;
        }

        xmlFreeDoc(xmlDoc);
        REQUIRE(numTests > 0);
        printf("</messagetest>\n");
    }
    REQUIRE(numFiles > 0);
    xmlCleanupParser();
}

