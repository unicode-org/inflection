/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/LoggerConfig.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include <iostream>

namespace morphuntion::util {

static MLogLevel gLogLevel = MLOG_INFO;

void
LoggerConfig::setLogLevel(MLogLevel newLogLevel) {
    gLogLevel = newLogLevel;
}

MLogLevel
LoggerConfig::getLogLevel() {
    return gLogLevel;
}

MORPHUNTION_CBEGIN
static void logClog(void *context, MLogLevel logLevel, const char16_t* category, const char16_t *message) {
    static const char logLevelPrefix[][12] = {
            "[TRACE] ",
            "[DEBUG] ",
            "[INFO] ",
            "[WARNING] ",
            "[ERROR] ",
            "[DISABLED] "
    };
    if (category != nullptr && logLevel <= MLOG_INFO) {
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
MORPHUNTION_CEND

void
LoggerConfig::logToConsole(bool value) {
    registerLogger(&std::clog, value ? &logClog : nullptr);
}

bool
LoggerConfig::isTraceEnabled() {
    return gLogLevel <= MLOG_TRACE;
}

bool
LoggerConfig::isDebugEnabled() {
    return gLogLevel <= MLOG_DEBUG;
}

bool
LoggerConfig::isInfoEnabled() {
    return gLogLevel <= MLOG_INFO;
}

bool
LoggerConfig::isWarnEnabled() {
    return gLogLevel <= MLOG_WARNING;
}

bool
LoggerConfig::isErrorEnabled() {
    return gLogLevel <= MLOG_ERROR;
}

bool
LoggerConfig::registerLogger(void* context, MLoggerCallback loggerCallback) {
    return Logger::registerLogger(context, loggerCallback);
}

bool
LoggerConfig::unregisterLogger(void* context) {
    return Logger::registerLogger(context, nullptr);
}

} // namespace morphuntion::util
