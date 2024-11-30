/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "catch2/catch_test_macros.hpp"

#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/LoggerConfig.h>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/npc.hpp>
#include <util/CFUtils.hpp>

static void checkForSuccess(CFErrorRef* error)
{
    REQUIRE(error != nullptr);
    REQUIRE(*error == nullptr);
}

MORPHUNTION_CBEGIN
static void stringLogger(void *context, MLogLevel logLevel, const char16_t* category, const char16_t *message) {
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
MORPHUNTION_CEND

TEST_CASE("LoggerConfigTest-c#testAPI", "[.]")
{
    std::vector<std::u16string> logLines;
    CFErrorRef error = nullptr;
    morphuntion::util::AutoCFRelease<CFLocaleRef> locale(::util::CFUtils::convert(::morphuntion::util::ULocale("tlh")));

    mlogc_registerLogger(&logLines, stringLogger, &error);
    checkForSuccess(&error);

    MDLocalizedCommonConceptFactoryProvider* localizedCommonConceptFactoryProvider = mlccfp_getDefaultCommonConceptFactoryProvider(&error);
    checkForSuccess(&error);

    mlccfp_getCommonConceptFactory(localizedCommonConceptFactoryProvider, locale.value, &error);
    checkForSuccess(&error);

    mlogc_unregisterLogger(&logLines, &error);
    checkForSuccess(&error);

    INFO(logLines.size());
    REQUIRE((logLines.size() == 1 || logLines.size() == 2));
    CHECK(morphuntion::util::StringViewUtils::startsWith(logLines[0], u"[INFO] [LocalizedCommonConceptFactoryProvider] The CommonConceptFactory for he is being constructed for the first time. Platform="));
    if (logLines.size() == 2) {
        // You might get this when running the test in isolation.
        INFO(morphuntion::util::StringUtils::to_string(logLines[1]));
        CHECK(morphuntion::util::StringViewUtils::startsWith(logLines[1], u"[INFO] [Tokenizer] The tokenizer for he is being constructed for the first time. Platform="));
    }

    logLines.clear();

    auto logLevel = mlogc_getLogLevel();
    mlogc_setLogLevel(MLOG_ERROR, &error);
    checkForSuccess(&error);

    REQUIRE(MLOG_ERROR == mlogc_getLogLevel());

    mlogc_setLogLevel(logLevel, &error);
    checkForSuccess(&error);

    REQUIRE(logLevel == mlogc_getLogLevel());
    REQUIRE(logLines.empty());
}
