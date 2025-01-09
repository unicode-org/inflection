/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "inflection/util/LogToString.hpp"
#include <inflection/util/LoggerConfig.hpp>

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
    LogToString* logToString = (::LogToString*)context;
    std::u16string line(logLevelPrefix[logLevel]);
    if (category != nullptr) {
        line = line + u"[" + std::u16string(category) + u"] ";
    }
    logToString->logLines.emplace_back(line + std::u16string(message));
}
INFLECTION_CEND

LogToString::LogToString() {
    inflection::util::LoggerConfig::registerLogger(this, stringLogger);
}

LogToString::~LogToString() {
    inflection::util::LoggerConfig::unregisterLogger(this);
}