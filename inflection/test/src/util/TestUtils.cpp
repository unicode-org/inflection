/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */

#include "util/TestUtils.hpp"
#include <catch2/catch_config.hpp>

#include <inflection/util/ULocale.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/resources/DataRegistrationService.hpp>
#include <inflection/npc.hpp>

#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace Catch::Clara;
using inflection::util::StringViewUtils;
using inflection::util::LocaleUtils;

namespace {
    inline void setInflectionLogLevel(std::string_view logLevelString) {
        const auto u16LogLevelString = StringViewUtils::to_u16string(logLevelString);
        std::u16string lowerLogString;
        StringViewUtils::lowercase(&lowerLogString, u16LogLevelString, LocaleUtils::ENGLISH());

        auto error = U_ZERO_ERROR;
        if (!lowerLogString.empty()) {
            // Set Inflection logging levels
            if (lowerLogString == u"trace") {
                ilogc_setLogLevel(ILOG_TRACE, &error);
            } else if(lowerLogString == u"debug") {
                ilogc_setLogLevel(ILOG_DEBUG, &error);
            } else if(lowerLogString == u"info") {
                ilogc_setLogLevel(ILOG_INFO, &error);
            } else if(lowerLogString == u"warning") {
                ilogc_setLogLevel(ILOG_WARNING, &error);
            } else if(lowerLogString == u"error") {
                ilogc_setLogLevel(ILOG_ERROR, &error);
            }
            assert(U_SUCCESS(error));
        }

        ilogc_logToConsole(true, &error);
        assert(U_SUCCESS(error));
    }
}

std::tuple<std::unique_ptr<Catch::Session>, int, int> TestUtils::createSession(int argc, const char* const argv[])
{
    // Register data paths from MobileAssets if necessary
    if (getenv("TEST_MOBILE_ASSETS") != nullptr) {
        for (const auto& languageMapping : inflection::util::LocaleUtils::getSupportedLocaleMap())
        {
            inflection::resources::DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale(languageMapping.first), "MobileAssets/" + languageMapping.first + "/AssetData");
        }
    } else {
#ifdef INFLECTION_ROOT
        // Setting inflection root from compiler defintion
        setenv("INFLECTION_ROOT", INFLECTION_ROOT, 0);
#endif
    }

    // Set-up catch session and custom CLI args
    std::unique_ptr<Catch::Session> session = std::make_unique<Catch::Session>();
    // Custom CLI args
    std::string testLangs;

    std::string logLevel;

    auto cli = session->cli()
        | Opt(testLangs, "Test languages")
             ["-l"]["--langs"]
             ("Comma-separated set of languages to test. Leave empty to test all supported languages.")
        | Opt(logLevel, "Log level")
            ["--log-level"]
            ("Sets the log level for running inflection tests(case-insensitive input), permitted values: trace, debug, info, warning, error");

    session->cli(cli);
    auto ret = session->applyCommandLine(argc, argv);
    setInflectionLogLevel(logLevel);

    auto originalLogLevel = ilogc_getLogLevel();
    if (ret) {
        return { std::move(session), ret, (int)originalLogLevel };
    }

    // Set the test language set
    TestUtils::setTestLocaleMapFromArg(testLangs);
    if (!testLangs.empty())
    {
        std::cout << "Testing language set: " << TestUtils::getTestLocaleMapPrintableString() << std::endl;
    }

    return { std::move(session), 0, (int)originalLogLevel };
}

static std::map<std::string, std::vector<inflection::util::ULocale>, std::greater<>>* constructLocaleMap()
{
    auto testLanguageSet = npc(new std::map<std::string, std::vector<inflection::util::ULocale>, std::greater<>>());

    for (const auto& [key, value] : inflection::util::LocaleUtils::getSupportedLocaleMap()) {
        npc(testLanguageSet)->emplace(key, value);
    }
    return testLanguageSet;
}

void TestUtils::cleanup()
{
    getTestLocaleMapInternal()->clear();
}

std::map<std::string, std::vector<inflection::util::ULocale>, std::greater<>>* TestUtils::getTestLocaleMapInternal()
{
    static auto testLanguageSet = constructLocaleMap();
    return testLanguageSet;
}

const std::map<std::string, std::vector<inflection::util::ULocale>, std::greater<>>& TestUtils::getTestLocaleMap()
{
    return *(getTestLocaleMapInternal());
}

std::string TestUtils::getTestLocaleMapPrintableString()
{
    std::stringstream ss;

    ss << '[';

    size_t i = 0;
    for(const auto& [language, localeArr] : TestUtils::getTestLocaleMap())
    {
        if (i != 0)
        {
            ss << ", ";
        }
        ss << language;
        i++;
    }

    ss << ']';

    return ss.str();
}

void TestUtils::setTestLocaleMapFromArg(std::string& langsArg)
{
    // Parse out test languages
    if (!langsArg.empty())
    {
        auto languageStrings = TestUtils::splitDelineatedString(langsArg, ",");
        auto supportedLocaleMap = inflection::util::LocaleUtils::getSupportedLocaleMap();
        auto testLocaleMapPtr = getTestLocaleMapInternal();
        testLocaleMapPtr->clear();
        for(std::string& lang : languageStrings)
        {
            testLocaleMapPtr->emplace(lang, supportedLocaleMap[lang]);
        }
    }
}

std::vector<std::string> TestUtils::splitDelineatedString(std::string& str, const std::string& delimiter)
{
    std::vector<std::string> tokens;

    size_t pos = 0;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        tokens.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }
    tokens.push_back(str);

    return tokens;
}

std::string
TestUtils::getTestResourcePath()
{
    std::string resourcePath;
    ::std::string srcDir(__FILE__);
    srcDir = srcDir.substr(0, srcDir.length() - ::std::string("/test/src/util/TestUtils.cpp").length());
    resourcePath = srcDir + "/test/resources/inflection/";
    return resourcePath;
}

::std::vector<::std::string> TestUtils::listDirectoryContents(const std::string& dirPath)
{
    ::std::vector<::std::string> files;
    DIR* dirptr = opendir(dirPath.c_str());
    if (dirptr == nullptr) {
        throw std::runtime_error(std::string("Not a valid path: ") + dirPath);
    }
    struct dirent * dirEnt;
    while ((dirEnt = readdir(dirptr)) != nullptr) {
        if (strcmp(dirEnt->d_name, ".") == 0 || strcmp(dirEnt->d_name, "..") == 0) {
            continue;
        }
        files.emplace_back(dirEnt->d_name);
    }
    closedir(dirptr);
    return files;
}
