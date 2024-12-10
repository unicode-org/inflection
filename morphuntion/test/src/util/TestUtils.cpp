/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */

#include "util/TestUtils.hpp"
#include <catch2/catch_config.hpp>

#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/resources/DataRegistrationService.hpp>
#include <morphuntion/npc.hpp>

#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace Catch::Clara;
using morphuntion::util::StringViewUtils;
using morphuntion::util::LocaleUtils;

namespace {
    inline void setMorphunLogLevel(std::string_view logLevelString) {
        const auto u16LogLevelString = StringViewUtils::to_u16string(logLevelString);
        std::u16string lowerLogString;
        StringViewUtils::lowercase(&lowerLogString, u16LogLevelString, LocaleUtils::ENGLISH());

        CFErrorRef error = nullptr;
        if (!lowerLogString.empty()) {
            // Set Morphuntion logging levels
            if (lowerLogString == u"trace") {
                mlogc_setLogLevel(MLOG_TRACE, &error);
            } else if(lowerLogString == u"debug") {
                mlogc_setLogLevel(MLOG_DEBUG, &error);
            } else if(lowerLogString == u"info") {
                mlogc_setLogLevel(MLOG_INFO, &error);
            } else if(lowerLogString == u"warning") {
                mlogc_setLogLevel(MLOG_WARNING, &error);
            } else if(lowerLogString == u"error") {
                mlogc_setLogLevel(MLOG_ERROR, &error);
            }
            assert(error == nullptr);
        }

        mlogc_logToConsole(true, &error);
        assert(error == nullptr);
    }
}

std::tuple<std::unique_ptr<Catch::Session>, int, int> TestUtils::createSession(int argc, const char* const argv[])
{
    // Register data paths from MobileAssets if necessary
    if (getenv("TEST_MOBILE_ASSETS") != nullptr) {
        for (const auto& languageMapping : morphuntion::util::LocaleUtils::getSupportedLocaleMap())
        {
            morphuntion::resources::DataRegistrationService::registerDataPathForLocale(morphuntion::util::ULocale(languageMapping.first), "MobileAssets/" + languageMapping.first + "/AssetData");
        }
    } else {
#ifdef MORPHUNTION_ROOT
        // Setting morphuntion root from compiler defintion
        setenv("MORPHUNTION_ROOT", MORPHUNTION_ROOT, 0);
#endif
    }

    // Set-up catch session and custom CLI args
    std::unique_ptr<Catch::Session> session = std::make_unique<Catch::Session>();
    // Custom CLI args
    std::string testLangs;

    std::string morphunLogLevel;

    auto cli = session->cli()
        | Opt(testLangs, "Test languages")
             ["-l"]["--langs"]
             ("Comma-separated set of languages to test. Leave empty to test all supported languages.")
        | Opt(morphunLogLevel, "Log level")
            ["--log-level"]
            ("Sets the log level for running morphuntion tests(case-insensitive input), permitted values: trace, debug, info, warning, error");

    session->cli(cli);
    auto ret = session->applyCommandLine(argc, argv);
    setMorphunLogLevel(morphunLogLevel);

    auto originalLogLevel = mlogc_getLogLevel();
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

static std::map<std::string, std::vector<morphuntion::util::ULocale>, std::greater<>>* constructLocaleMap()
{
    auto testLanguageSet = npc(new std::map<std::string, std::vector<morphuntion::util::ULocale>, std::greater<>>());

    for (const auto& [key, value] : morphuntion::util::LocaleUtils::getSupportedLocaleMap()) {
        npc(testLanguageSet)->emplace(key, value);
    }
    return testLanguageSet;
}

void TestUtils::cleanup()
{
    getTestLocaleMapInternal()->clear();
}

std::map<std::string, std::vector<morphuntion::util::ULocale>, std::greater<>>* TestUtils::getTestLocaleMapInternal()
{
    static auto testLanguageSet = constructLocaleMap();
    return testLanguageSet;
}

const std::map<std::string, std::vector<morphuntion::util::ULocale>, std::greater<>>& TestUtils::getTestLocaleMap()
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
        auto supportedLocaleMap = morphuntion::util::LocaleUtils::getSupportedLocaleMap();
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
    resourcePath = srcDir + "/test/resources/morphuntion/";
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
