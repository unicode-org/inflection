/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/LoggerConfig.h>
#include <inflection/npc.hpp>

INFLECTION_CBEGIN
static void stringLogger(void *context, ILogLevel logLevel, const char16_t* category, const char16_t *message) {
    static const char16_t * const logLevelPrefix[] = {
            u"[TRACE] ",
            u"[DEBUG] ",
            u"[INFO] ",
            u"[WARNING] ",
            u"[ERROR] ",
            u"[DISABLED] "
    };
    std::vector<std::u16string>* logLines = (::std::vector<std::u16string>*)context;
    std::u16string line(logLevelPrefix[logLevel]);
    if (category != nullptr) {
        line = line + u"[" + std::u16string(category) + u"] ";
    }
    npc(logLines)->emplace_back(line + std::u16string(message));
}
INFLECTION_CEND

TEST_CASE("LoggerConfigTest-c#testAPI", "[.]")
{
    std::vector<std::u16string> logLines;
    auto error = U_ZERO_ERROR;
    auto locale = "tlh";

    ilogc_registerLogger(&logLines, stringLogger, &error);
    REQUIRE(U_SUCCESS(error));

    IDLocalizedCommonConceptFactoryProvider* localizedCommonConceptFactoryProvider = ilccfp_getDefaultCommonConceptFactoryProvider(&error);
    REQUIRE(U_SUCCESS(error));

    ilccfp_getCommonConceptFactory(localizedCommonConceptFactoryProvider, locale, &error);
    REQUIRE(U_SUCCESS(error));

    ilogc_unregisterLogger(&logLines, &error);
    REQUIRE(U_SUCCESS(error));

    INFO(logLines.size());
    REQUIRE((logLines.size() == 1 || logLines.size() == 2));
    CHECK(logLines[0].starts_with(u"[INFO] [LocalizedCommonConceptFactoryProvider] The CommonConceptFactory for he is being constructed for the first time. Platform="));
    if (logLines.size() == 2) {
        // You might get this when running the test in isolation.
        INFO(inflection::util::StringUtils::to_string(logLines[1]));
        CHECK(logLines[1].starts_with(u"[INFO] [Tokenizer] The tokenizer for he is being constructed for the first time. Platform="));
    }

    logLines.clear();

    auto logLevel = ilogc_getLogLevel();
    ilogc_setLogLevel(ILOG_ERROR, &error);
    REQUIRE(U_SUCCESS(error));

    REQUIRE(ILOG_ERROR == ilogc_getLogLevel());

    ilogc_setLogLevel(logLevel, &error);
    REQUIRE(U_SUCCESS(error));

    REQUIRE(logLevel == ilogc_getLogLevel());
    REQUIRE(logLines.empty());
}
