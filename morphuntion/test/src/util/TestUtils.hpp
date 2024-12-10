/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/LoggerConfig.h>
#include <morphuntion/util/ULocale.hpp>

#include <catch2/catch_session.hpp>

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <memory>

class TestUtils final
{
public:
    static std::tuple<std::unique_ptr<Catch::Session>, int, int> createSession(int argc, const char* const argv[]);

    static std::string getTestResourcePath();
    static void setTestLocaleMapFromArg(std::string& langsArg);

    // Maps from language -> [locales]
    static const std::map<std::string, std::vector<morphuntion::util::ULocale>, std::greater<>>& getTestLocaleMap();
    static void cleanup();
    static std::string getTestLocaleMapPrintableString();
    static ::std::vector<::std::string> listDirectoryContents(const std::string& dirPath);

private:
    TestUtils() = delete;

    static std::map<std::string, std::vector<morphuntion::util::ULocale>, std::greater<>>* getTestLocaleMapInternal();
    static std::vector<std::string> splitDelineatedString(std::string& str, const std::string& delimiter);
};
