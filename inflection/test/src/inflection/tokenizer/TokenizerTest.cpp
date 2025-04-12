/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/tokenizer/Token.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenizerFactory.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/LogToString.hpp>
#include <inflection/util/MemoryMappedFile.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/UnicodeSet.hpp>
#include <util/TestUtils.hpp>
#include <unicode/uloc.h>
#include <dirent.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <algorithm>
#include <memory>
#include <list>

template<typename T, typename U>
static T java_cast(U* u)
{
    if(!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if(!t) throw inflection::exception::ClassCastException();
    return t;
}

static std::string deriveSourceText(const ::std::vector<::std::u16string>& tokens) {
    std::string result;
    for (const auto& token : tokens) {
        if (!result.empty() && !token.empty()) {
            result += "|";
        }
        result += inflection::util::StringUtils::to_string(token);
    }
    return result;
}

void compareTokens(::inflection::tokenizer::TokenChain* tokChainLocal, const std::string& locale, bool compareClean, const ::std::vector<::std::u16string>& expectedResults)
{
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokChain(tokChainLocal);
    if (int32_t(expectedResults.size()) != tokChain->getSize()) {
        std::string msg("Got: " + inflection::util::StringUtils::to_string(tokChain->toString())
                        + "\nExpected: " + deriveSourceText(expectedResults)
                        + "\nLocale: " + locale);
        INFO(msg);
        CHECK((int32_t)expectedResults.size() == tokChain->getSize());
    }
    else {
        int32_t resultsIdx = 0;
        for (auto tok = tokChain->begin(); tok != tokChain->end(); ++tok) {
            auto value = compareClean ? tok->getCleanValue() : tok->getValue();
            auto comparisonResult = expectedResults[resultsIdx++].compare(value);
            if (comparisonResult != 0) {
                std::string msg("Got: " + inflection::util::StringUtils::to_string(tokChain->toString())
                                + "\nExpected: " + deriveSourceText(expectedResults)
                                + "\nToken: -> " + inflection::util::StringUtils::to_string(expectedResults[resultsIdx]) + " != " + inflection::util::StringUtils::to_string(value));
                if (!locale.empty()) {
                    msg += " for locale=" + locale;
                }
                puts(msg.c_str());
            }
            CHECK(comparisonResult == 0);
        }
    }
}

void compareValueTokens(::inflection::tokenizer::TokenChain* tokChain, const ::std::vector<::std::u16string>& expectedResults)
{
    compareTokens(tokChain, {}, false, expectedResults);
}

void compareCleanTokens(::inflection::tokenizer::TokenChain* tokChain, const ::std::vector<::std::u16string>& expectedResults)
{
    compareTokens(tokChain, {}, true, expectedResults);
}

void list_directory_contents(const std::string& dirPath, ::std::vector<::std::string> &files)
{
    DIR* dirptr = opendir(dirPath.c_str());
    struct dirent * dirEnt;
    while ((dirEnt = readdir(dirptr)) != nullptr) {
        files.push_back(dirEnt->d_name);
    }
    closedir(dirptr);
}
static void getContent(::std::u16string* result, xmlNodePtr nodePtr) {
    xmlChar* content = xmlNodeGetContent(nodePtr);
    ::inflection::util::StringUtils::convert(result, (const char*)content);
    xmlFree(content);
}

TEST_CASE("TokenizerTest#testDefault")
{
    LogToString stringLogger;
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::ULocale("zz")));
    REQUIRE((stringLogger.logLines.size() == 1 || stringLogger.logLines.size() == 2));
    INFO(inflection::util::StringUtils::to_string(stringLogger.logLines[0]));
    CHECK(stringLogger.logLines[0] == u"[WARNING] The tokenizer for zz is unknown. The default will be used.");
    if (stringLogger.logLines.size() == 2) {
        // You might get this when running the test in isolation.
        INFO(inflection::util::StringUtils::to_string(stringLogger.logLines[1]));
        CHECK(stringLogger.logLines[1].starts_with(u"[INFO] [Tokenizer] The tokenizer for zz is being constructed for the first time. Platform="));
    }
    compareValueTokens(tokenizer->createTokenChain(u"Hello world!"), {u"", u"Hello", u" ", u"world", u"!", u""});
}

TEST_CASE("TokenizerTest#testToString")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(inflection::util::LocaleUtils::ENGLISH()));
    auto result(tokenizer->toString());
    // This is the default C++ object toString behavior. It can appear as N7inflection9TokenizerE on macOS with clang and libc++.
    CHECK(result.find(u"Tokenizer") != std::u16string::npos);
    CHECK(result.find(u"inflection") != std::u16string::npos);
}

TEST_CASE("TokenizerTest#testAliases")
{
    std::vector<inflection::util::ULocale> locales({{"iw"}, {"id"}, {"no"}, {"cmn_Hans_CN@sd=cnsc"},
                                                 {inflection::util::LocaleUtils::INDONESIA()},
                                                 {inflection::util::LocaleUtils::SHANGHAINESE()},
                                                 {inflection::util::LocaleUtils::CHINA_SHANGHAINESE()},
                                                 {inflection::util::LocaleUtils::SIMPLIFIED_CHINESE()},
    });
    for (const auto& locale : locales) {
        ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(locale));
        compareValueTokens(tokenizer->createTokenChain(u"Hello world!"), {u"", u"Hello", u" ", u"world", u"!", u""});
    }
}

TEST_CASE("TokenizerTest#testDashes")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    compareValueTokens(tok->createTokenChain(u"-aaa"), {u"", u"-", u"aaa", u""});
    compareValueTokens(tok->createTokenChain(u"aaa-"), {u"", u"aaa", u"-", u""});
    compareValueTokens(tok->createTokenChain(u"aaa- b"), {u"", u"aaa", u"-", u" ", u"b", u""});
    compareValueTokens(tok->createTokenChain(u"pg-13-rated"), {u"", u"pg", u"-", u"13", u"-", u"rated", u""});
    compareValueTokens(tok->createTokenChain(u"pg-13 in 2-d"), {u"", u"pg", u"-", u"13", u" ", u"in", u" ", u"2", u"-", u"d", u""});
    compareValueTokens(tok->createTokenChain(u"aaa pg-13-rated"), {u"", u"aaa", u" ", u"pg", u"-", u"13", u"-", u"rated", u""});
    compareValueTokens(tok->createTokenChain(u"aaa pg-13-rated-foo"), {u"", u"aaa", u" ", u"pg", u"-", u"13", u"-", u"rated", u"-", u"foo", u""});
    compareValueTokens(tok->createTokenChain(u"aaa pg-13-rated-foo-"), {u"", u"aaa", u" ", u"pg", u"-", u"13", u"-", u"rated", u"-", u"foo", u"-", u""});
    compareValueTokens(tok->createTokenChain(u"aaa -pg-13-rated-foo"), {u"", u"aaa", u" ", u"-", u"pg", u"-", u"13", u"-", u"rated", u"-", u"foo", u""});
    compareValueTokens(tok->createTokenChain(u"aaa -pg-13--rated-foo"), {u"", u"aaa", u" ", u"-", u"pg", u"-", u"13", u"-", u"-", u"rated", u"-", u"foo", u""});
    compareValueTokens(tok->createTokenChain(u"-aaa-"), {u"", u"-", u"aaa", u"-", u""});
    compareValueTokens(tok->createTokenChain(u"aaa--bbb"), {u"", u"aaa", u"-", u"-", u"bbb", u""});
    compareValueTokens(tok->createTokenChain(u"aaa-bbb xxx"), {u"", u"aaa", u"-", u"bbb", u" ", u"xxx", u""});
    compareValueTokens(tok->createTokenChain(u"aaa---bbb xxx"), {u"", u"aaa", u"-", u"-", u"-", u"bbb", u" ", u"xxx", u""});
    compareValueTokens(tok->createTokenChain(u"xxx aaa--bbb"), {u"", u"xxx", u" ", u"aaa", u"-", u"-", u"bbb", u""});
    compareValueTokens(tok->createTokenChain(u"1-1 22-2 3-33 44-44"), {u"", u"1", u"-", u"1", u" ", u"22", u"-", u"2", u" ", u"3", u"-", u"33", u" ", u"44", u"-", u"44", u""});
}

TEST_CASE("TokenizerTest#testPunctuation")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(npc(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US())));
    compareValueTokens(tokenizer->createTokenChain(u"this is a \"simple-test\" again"), {u"", u"this", u" ", u"is", u" ", u"a", u" ", u"\"", u"simple", u"-", u"test", u"\"", u" ", u"again", u""});
    compareValueTokens(tokenizer->createTokenChain(u"I like   $100  punctuation !?.,\"'"), {u"", u"I", u" ", u"like", u"   ", u"$", u"100", u"  ", u"punctuation", u" ", u"!", u"?", u".", u",", u"\"", u"\'", u""});
    compareValueTokens(tokenizer->createTokenChain(u"9:00 e:e"), {u"", u"9", u":", u"00", u" ", u"e:e", u""});
}

TEST_CASE("TokenizerTest#testAlphaNumericTokens")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    compareValueTokens(tokenizer->createTokenChain(u"at 2x"), {u"", u"at", u" ", u"2", u"x", u""});
    compareValueTokens(tokenizer->createTokenChain(u"2x"), {u"", u"2", u"x", u""});
    compareValueTokens(tokenizer->createTokenChain(u"x2"), {u"", u"x", u"2", u""});
    compareValueTokens(tokenizer->createTokenChain(u"3! * sin(76) + ln(361)^3 - 2*csc(138)"), {u"", u"3", u"!", u" ", u"*", u" ", u"sin", u"(", u"76", u")", u" ", u"+", u" ", u"ln", u"(", u"361", u")", u"^", u"3", u" ", u"-", u" ", u"2", u"*", u"csc", u"(", u"138", u")", u""});
}

bool isSignificant(::inflection::tokenizer::TokenChain* tokChain, int32_t index)
{
    auto token = npc(tokChain)->getToken(index);
    if (token == nullptr) {
        throw inflection::exception::NullPointerException();
    }
    return npc(token)->isSignificant();
}

TEST_CASE("TokenizerTest#testSignificantTokens")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::FRENCH()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(u"1 + 1"));
    REQUIRE(isSignificant(tokenChain.get(), 0));
    REQUIRE_FALSE(isSignificant(tokenChain.get(), 1));
    REQUIRE(isSignificant(tokenChain.get(), 2));
    REQUIRE_FALSE(isSignificant(tokenChain.get(), 3));
    REQUIRE(isSignificant(tokenChain.get(), 4));
    tokenChain.reset(tokenizer->createTokenChain(u"dash-dash"));
    REQUIRE(isSignificant(tokenChain.get(), 0));
    REQUIRE_FALSE(isSignificant(tokenChain.get(), 4));
}

TEST_CASE("TokenizerTest#testWhiteSpaces")
{
    icu4cxx::UnicodeSet set(u"[:whitespace:]");
    int32_t size = set.size();
    for (int idx = 0; idx < size; idx++) {
        REQUIRE(u_isUWhiteSpace(set.charAt(idx)));
    }
    REQUIRE_FALSE(u_isUWhiteSpace(u'A'));

    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tk(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::list<::std::u16string> whitespaces {
        u"\u000A", u"\u000B", u"\u000C", u"\u000D", u"\u0009", u"\u0020",
        u"\u00a0", u"\u1680", u"\u2000", u"\u2001", u"\u2002", u"\u2003",
        u"\u2004", u"\u2005", u"\u2006", u"\u2007", u"\u2008", u"\u2009",
        u"\u200A", u"\u202F", u"\u205F", u"\u2028", u"\u2029", u"\u3000"
    };

    for (::std::u16string whitespace : whitespaces) {
        auto tokenChain = tk->createTokenChain(whitespace);
        REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(npc(npc(tokenChain)->getHead())->getNext()) != nullptr);
        REQUIRE((npc(npc(tokenChain)->getHead())->getNext()->isWhitespace()) == true);
        compareValueTokens(tokenChain, {u"", whitespace, u""});
    }

    auto tokenChain = tk->createTokenChain(u"hello\u2009world");
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(npc(npc(npc(tokenChain)->getHead())->getNext())->getNext()) != nullptr);
    REQUIRE((npc(npc(npc(tokenChain)->getHead())->getNext())->getNext()->isWhitespace()) == true);
    compareValueTokens(tokenChain, {u"", u"hello", u"\u2009", u"world", u""});

    tokenChain = tk->createTokenChain(u"another \u2005 whitespace");
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(npc(npc(npc(tokenChain)->getHead())->getNext())->getNext()) != nullptr);
    REQUIRE((npc(npc(npc(tokenChain)->getHead())->getNext())->getNext()->isWhitespace()) == true);
    compareValueTokens(tokenChain, {u"", u"another", u" \u2005 ", u"whitespace", u""});

    tokenChain = tk->createTokenChain(u"consecutive \u2007 \u0020 whitespaces");
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(npc(npc(npc(npc(tokenChain)->getHead())->getNext())->getNext())->getNext()) != nullptr);
    REQUIRE((npc(npc(npc(npc(tokenChain)->getHead())->getNext())->getNext())->getNext()->isWhitespace()) == true);
    compareValueTokens(tokenChain, {u"", u"consecutive", u" ", u"\u2007", u" \u0020 ", u"whitespaces", u""});

    tokenChain = tk->createTokenChain(u"\u2002\u2004 joint whitespaces");
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(npc(npc(tokenChain)->getHead())->getNext()) != nullptr);
    REQUIRE((npc(npc(tokenChain)->getHead())->getNext()->isWhitespace()) == true);
    compareValueTokens(tokenChain, {u"", u"\u2002\u2004 ", u"joint", u" ", u"whitespaces", u""});
}

TEST_CASE("TokenizerTest#testFrenchTokenizer")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tk(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::FRENCH()));
    auto tokenChain = tk->createTokenChain(u"\u00E1 la maison");
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(npc(npc(tokenChain)->getHead())->getNext()) != nullptr);
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(npc(npc(npc(tokenChain)->getHead())->getNext())->getNext()) != nullptr);
    compareValueTokens(tokenChain, {u"", u"\u00E1", u" ", u"la", u" ", u"maison", u""});
    compareValueTokens(tk->createTokenChain(u"a la maison"), {u"", u"a", u" ", u"la", u" ", u"maison", u""});
}

TEST_CASE("TokenizerTest#testModifiers")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    compareValueTokens(tokenizer->createTokenChain(u"9AM HAL9000"), {u"", u"9", u"AM", u" ", u"HAL", u"9000", u""});
    tokenizer->setStyle(inflection::tokenizer::Tokenizer::TokenizationType::UNMODIFIED);
    compareValueTokens(tokenizer->createTokenChain(u"9AM HAL9000"), {u"", u"9AM", u" ", u"HAL9000", u""});
}

void checkSingleToken(::inflection::tokenizer::Tokenizer* tokenizer, const std::string& locale, const ::std::u16string& annotatedTokens)
{
    ::std::u16string input;
    inflection::util::StringViewUtils::replace(&input, annotatedTokens, u"|", u"");
    auto tokenChain = npc(tokenizer)->createTokenChain(input);
    auto expected = inflection::util::StringViewUtils::split(annotatedTokens, u"|");
    expected.insert(expected.begin(), u"");
    expected.emplace_back(u"");

    compareTokens(tokenChain, locale, false, expected);
}

void checkSingleToken(const ::std::unique_ptr<::inflection::tokenizer::Tokenizer>& tokenizer, const ::std::u16string& annotatedTokens)
{
    checkSingleToken(tokenizer.get(), {}, annotatedTokens);
}

TEST_CASE("TokenizerTest#testSingleToken")
{
    const auto &testLocales = TestUtils::getTestLocaleMap();
    std::string resourcePath = TestUtils::getTestResourcePath() + "tokenizer/";
    ::std::vector<::std::string> files;
    list_directory_contents(resourcePath, files);
    sort(files.begin(), files.end());
    int32_t numFiles = 0;
    for (const auto& file : files) {
        if (file == "." || file == "..") {
            continue;
        }
        ::std::u16string resourceFile;
        inflection::util::StringUtils::convert(&resourceFile, resourcePath+file);
        ::inflection::util::MemoryMappedFile mMapFile(resourceFile);
        xmlDocPtr xmlDoc = xmlParseMemory(mMapFile.getData(), (int)mMapFile.getSize());
        if (xmlDoc == nullptr) {
            throw ::inflection::exception::IOException(resourceFile);
        }
        xmlNodePtr tokenizerTest = xmlDocGetRootElement(xmlDoc);
        if (tokenizerTest == nullptr) {
            throw ::inflection::exception::XMLParseException(u"Incorrect root element -- expecting <tokenizerTest>, got <null>");
        }

        ::std::string locale = file.substr(0, file.length() - ::std::string(".xml").length());
        ::inflection::util::ULocale ulocale(locale);

        if (testLocales.find(ulocale.getLanguage()) == testLocales.end()) {
            continue;
        }

        ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(ulocale));

        numFiles++;
        int32_t numTests = 0;
        for (xmlNodePtr currTestNode = tokenizerTest->children; currTestNode != nullptr; currTestNode = currTestNode->next) {
            if (currTestNode->type != XML_ELEMENT_NODE) {
                continue;
            }
            if (xmlStrEqual(currTestNode->name, (const xmlChar *) "test") == 0) {
                throw ::inflection::exception::XMLParseException(u"Expecting element <test>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(currTestNode->name))) + u">");
            }
            xmlNodePtr testCaseNode = xmlFirstElementChild(currTestNode);
            if (xmlStrEqual(testCaseNode->name, (const xmlChar *) "annotatedTokens") == 0) {
                throw ::inflection::exception::XMLParseException(u"Expecting element <annotatedTokens>, got <" + ::inflection::util::StringUtils::to_u16string(std::string(reinterpret_cast<const char*>(testCaseNode->name))) + u">");
            }
            ::std::u16string annotatedTokens;
            getContent(&annotatedTokens, testCaseNode);
            checkSingleToken(tokenizer.get(), locale, annotatedTokens);
            numTests++;
        }
        xmlFreeDoc(xmlDoc);
        REQUIRE(numTests > 0);
    }
    REQUIRE(numFiles > 0);
    xmlCleanupParser();
}

TEST_CASE("TokenizerTest#testNumberSplits")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> englishTok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tok(englishTok->createTokenChain(u"IΣΣ9"));
    REQUIRE(u"IΣΣ" == npc(npc(tok->getHead())->getNext())->getValue());
    REQUIRE(u"iσς" == npc(npc(tok->getHead())->getNext())->getCleanValue());
    tok.reset(englishTok->createTokenChain(u"2PM"));
    REQUIRE(u"PM" == npc(npc(tok->getTail())->getPrevious())->getValue());
    REQUIRE(u"pm" == npc(npc(tok->getTail())->getPrevious())->getCleanValue());
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> turkishTok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::TURKISH()));
    tok.reset(turkishTok->createTokenChain(u"9I"));
    REQUIRE(u"I" == npc(npc(tok->getTail())->getPrevious())->getValue());
    REQUIRE(u"ı" == npc(npc(tok->getTail())->getPrevious())->getCleanValue());
}

TEST_CASE("TokenizerTest#testLanguageSpecificSplits")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> englishTok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::US()));
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> frenchTok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::FRANCE()));
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> germanTok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::GERMANY()));
    compareValueTokens(englishTok->createTokenChain(u"L'Apple-television"), {u"", u"L'Apple", u"-", u"television", u""});
    compareValueTokens(englishTok->createTokenChain(u"Create an alarm called 'hal9000'"), {u"", u"Create", u" ", u"an", u" ", u"alarm", u" ", u"called", u" ", u"'", u"hal", u"9000", u"'", u""});
    compareValueTokens(frenchTok->createTokenChain(u"L'Apple-television"), {u"", u"L", u"'", u"Apple", u"-", u"television", u""});
    compareValueTokens(germanTok->createTokenChain(u"L'Apple-television"), {u"", u"L'Apple", u"-", u"television", u""});
    compareValueTokens(germanTok->createTokenChain(u"L'Apple--television"), {u"", u"L'Apple", u"-", u"-", u"television", u""});
    compareValueTokens(germanTok->createTokenChain(u"e-mail"), {u"", u"e", u"-", u"mail", u""});
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokenChain;
    tokenChain.reset(germanTok->createTokenChain(u"1-1"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(1)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u":-)"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(1)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u"-1"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(0)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u"-a"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(0)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u"1-"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(1)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u"a-"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(tokenChain->getToken(1)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u"の-"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(1)) != nullptr);
    tokenChain.reset(englishTok->createTokenChain(u"a-a"));
    REQUIRE(2 == tokenChain->getWordCount());
    tokenChain.reset(englishTok->createTokenChain(u"1-1"));
    REQUIRE(2 == tokenChain->getWordCount());
    tokenChain.reset(germanTok->createTokenChain(u"a-a"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Delim* >(tokenChain->getToken(1)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u"What is -1 + 1"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(8)) != nullptr);
    tokenChain.reset(germanTok->createTokenChain(u"What is 1 - 1"));
    REQUIRE(dynamic_cast< ::inflection::tokenizer::Token_Word* >(tokenChain->getToken(10)) != nullptr);
}

TEST_CASE("TokenizerTest#testSwedishDictionary")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::SWEDEN()));
    compareValueTokens(tokenizer->createTokenChain(u"kärnvapennedrustningsorganisationen"), {u"", u"kärnvapen", u"nedrustning", u"s", u"organisationen", u""});
    compareValueTokens(tokenizer->createTokenChain(u"Näringslivsminister"), {u"", u"Näring", u"s", u"liv", u"s", u"minister", u""});
    compareValueTokens(tokenizer->createTokenChain(u"Nordkalottstaden"), {u"", u"Nord", u"kalott", u"staden", u""});
    compareValueTokens(tokenizer->createTokenChain(u"Klara Norra Kyrkogata"), {u"", u"Klara", u" ", u"Norra", u" ", u"Kyrk", u"o", u"gata", u""});
}

TEST_CASE("TokenizerTest#testDutchDictionary")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::NETHERLANDS()));
    compareValueTokens(tokenizer->createTokenChain(u"wifiinstellingsknop"), {u"", u"wifi", u"instelling", u"s", u"knop", u""});
    compareValueTokens(tokenizer->createTokenChain(u"bluetoothinstelling"), {u"", u"bluetooth", u"instelling", u""});
}

TEST_CASE("TokenizerTest#testItalianTokenizer")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ITALIAN()));
    compareValueTokens(tokenizer->createTokenChain(u"po’ l’ora"), {u"", u"po", u"’", u" ", u"l", u"’", u"ora", u""});
}

TEST_CASE("TokenizerTest#testArabic")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ARABIC()));
    compareValueTokens(tokenizer->createTokenChain(u"قطة قط قطط قطتان قطان قطتنا قطتها قطتهم قطتك قطتكن قطته قطتكما قطتي قططنا قططي"), {u"", u"قطة", u" ", u"قط", u" ", u"قطط", u" ", u"قطتان", u" ", u"قطان", u" ", u"قطت", u"نا", u" ", u"قطت", u"ها", u" ", u"قطت", u"هم", u" ", u"قطت", u"ك", u" ", u"قطت", u"كن", u" ", u"قطت", u"ه", u" ", u"قطت", u"كما", u" ", u"قطتي", u" ", u"قططنا", u" ", u"قطط", u"ي", u""});
    compareValueTokens(tokenizer->createTokenChain(u"ال فيس بوك"), {u"", u"ال", u" ", u"فيس", u" ", u"بوك", u""});
    compareCleanTokens(tokenizer->createTokenChain(u"قطة قط قطط قطتان قطان قطتنا قطتها قطتهم قطتك قطتكن قطته قطتكما قطتي قططنا قططي"), {u"", u"قطة", u" ", u"قط", u" ", u"قطط", u" ", u"قطتان", u" ", u"قطان", u" ", u"قطة", u"نا", u" ", u"قطة", u"ها", u" ", u"قطة", u"هم", u" ", u"قطة", u"ك", u" ", u"قطة", u"كن", u" ", u"قطة", u"ه", u" ", u"قطة", u"كما", u" ", u"قطتي", u" ", u"قططنا", u" ", u"قطط", u"ي", u""});
    compareCleanTokens(tokenizer->createTokenChain(u"فيس بوك فيسبوك الفيسبوك"), {u"", u"فيس", u" ", u"بوك", u" ", u"فيسبوك", u" ", u"ال", u"فيسبوك", u""});
    compareCleanTokens(tokenizer->createTokenChain(u"درزنات ثلاث ونصف درزنات"), {u"", u"درزنات", u" ", u"ثلاث", u" ", u"و", u"نصف", u" ", u"درزنات", u""});
}

TEST_CASE("TokenizerTest#testHebrew")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> hebTok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::HEBREW()));
    compareValueTokens(hebTok->createTokenChain(u"תתקשר ל-Eric"), {u"", u"תתקשר", u" ", u"ל", u"-", u"Eric", u""});
    compareValueTokens(hebTok->createTokenChain(u"תתקשר ל-911"), {u"", u"תתקשר", u" ", u"ל", u"-", u"911", u""});
}

TEST_CASE("TokenizerTest#testNewInstance")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ENGLISH()));
    tokenizer->setStyle(inflection::tokenizer::Tokenizer::DEFAULT);
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokChain(tokenizer->createTokenChain(u"I have an explicit ^key^=^value^! OVERSIZED_TOKEN"));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> cloneChain(tokChain->getNewTokenChain(tokChain->getHead()->getStartChar(), tokChain->getTail()->getEndChar()));
    if (cloneChain->getSize() != tokChain->getSize()) {
        INFO(inflection::util::StringUtils::to_string(tokChain->toString()));
    }
    REQUIRE(cloneChain->getSize() == tokChain->getSize());
    auto cloneTok = cloneChain->begin();
    auto cloneTokEnd = cloneChain->end();
    auto tok = tokChain->begin();
    auto tokEnd = tokChain->end();
    for (; tok != tokEnd && cloneTok != cloneTokEnd; ++tok, ++cloneTok) {
        REQUIRE(tok.get() != cloneTok.get());
        auto comparisonResult = *npc(tok.get()) == *npc(cloneTok.get());
        if (!comparisonResult) {
            INFO(inflection::util::StringUtils::to_string(tokChain->toString() + u" -> " + tok->getValue() + u" != " + cloneTok->getValue()));
        }
        REQUIRE(comparisonResult);
    }
    REQUIRE(tok.get() == nullptr);
    REQUIRE(cloneTok.get() == nullptr);
    tokChain.reset(nullptr); // delete first.
}

TEST_CASE("TokenizerTest#testLocaleCoverage")
{
    std::u16string displayName;
    // Ensure that nothing crashes or logs warnings if we're advertising support.
    for (const auto& entry : TestUtils::getTestLocaleMap()) {
        inflection::util::ULocale language(entry.first);
        ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(language));
        ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokChain(tokenizer->createTokenChain(language.toString()));
        for (const auto& locale : entry.second) {
            ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizerLocale(::inflection::tokenizer::TokenizerFactory::createTokenizer(locale));
            auto status = U_ZERO_ERROR;
            displayName.assign(ULOC_FULLNAME_CAPACITY, 0);
            auto resultSize = uloc_getDisplayName(locale.getName().c_str(), locale.getName().c_str(), (UChar*)displayName.data(), (int32_t)displayName.size(), &status);
            ::inflection::exception::ICUException::throwOnFailure(status);
            displayName.resize(resultSize);
            ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokChainLocale(tokenizer->createTokenChain(displayName));
            // If tokenizing the name of the locale display name doesn't consume the entire string, something very bad has happened.
            CHECK(tokChainLocale->getEnd()->getEndChar() == resultSize);
            CHECK(tokChainLocale->getStart()->getStartChar() == 0);
            CHECK(tokChainLocale->toOriginatingString() == displayName);
        }
    }
}

TEST_CASE("TokenizerTest#testKeywords")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> english(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::ULocale("en_US@calendar=japanese")));
    checkSingleToken(english, u"Hello| |world|!");
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> sichuanese(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::ULocale("cmn_Hans_CN@sd=cnsc")));
    checkSingleToken(sichuanese, u"Hello| |world|!");
}

TEST_CASE("TokenizerTest#testLongText")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tok(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::TRADITIONAL_CHINESE()));
    ::std::u16string longInput (300000, 'A');
    compareValueTokens(tok->createTokenChain(longInput), {u"", longInput, u""});
}

TEST_CASE("TokenizerTest#testIteration")
{
    ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer(::inflection::tokenizer::TokenizerFactory::createTokenizer(::inflection::util::LocaleUtils::ENGLISH()));
    ::std::unique_ptr<::inflection::tokenizer::TokenChain> tokChain(tokenizer->createTokenChain(u"a b c d e f"));
    auto iterator(tokChain->begin());
    CHECK((++iterator)->getValue() == u"a");
    CHECK((iterator++)->getValue() == u"a");
    CHECK(iterator->getValue() == u" ");
    ++iterator;
    CHECK(iterator != tokChain->begin());
    CHECK(iterator->getValue() == u"b");
    CHECK((iterator--)->getValue() == u"b");
    CHECK((--iterator)->getValue() == u"a");
    CHECK((--iterator) == tokChain->begin());
}
