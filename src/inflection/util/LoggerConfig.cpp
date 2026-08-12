/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <iostream>

namespace inflection::util {

static ILogLevel gLogLevel = ILOG_INFO;

void
LoggerConfig::setLogLevel(ILogLevel newLogLevel) {
    gLogLevel = newLogLevel;
}

ILogLevel
LoggerConfig::getLogLevel() {
    return gLogLevel;
}

INFLECTION_CBEGIN
static void logClog(void *context, ILogLevel logLevel, const char16_t* category, const char16_t *message) {
    static const char logLevelPrefix[][12] = {
            "[TRACE] ",
            "[DEBUG] ",
            "[INFO] ",
            "[WARNING] ",
            "[ERROR] ",
            "[DISABLED] "
    };
    if (category != nullptr && logLevel <= ILOG_INFO) {
        // Ignore the metrics
        return;
    }
    ::std::string str;
    ::std::ostream* logStream = npc((::std::ostream*)context);
    *logStream << logLevelPrefix[logLevel];
    if (category != nullptr) {
        StringViewUtils::convert(&str, category);
        *logStream << "[" << str << "] ";
    }
    StringViewUtils::convert(&str, message);
    *logStream << str << ::std::endl;
}
INFLECTION_CEND

void
LoggerConfig::logToConsole(bool value) {
    registerLogger(&std::clog, value ? &logClog : nullptr);
}

bool
LoggerConfig::isTraceEnabled() {
    return gLogLevel <= ILOG_TRACE;
}

bool
LoggerConfig::isDebugEnabled() {
    return gLogLevel <= ILOG_DEBUG;
}

bool
LoggerConfig::isInfoEnabled() {
    return gLogLevel <= ILOG_INFO;
}

bool
LoggerConfig::isWarnEnabled() {
    return gLogLevel <= ILOG_WARNING;
}

bool
LoggerConfig::isErrorEnabled() {
    return gLogLevel <= ILOG_ERROR;
}

bool
LoggerConfig::registerLogger(void* context, ILoggerCallback loggerCallback) {
    return Logger::registerLogger(context, loggerCallback);
}

bool
LoggerConfig::unregisterLogger(void* context) {
    return Logger::registerLogger(context, nullptr);
}

} // namespace inflection::util
