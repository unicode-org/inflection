/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/LoggerConfig.h>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/CFUtils.hpp>

using ::morphuntion::util::LoggerConfig;
using ::morphuntion::util::CFUtils;

MORPHUNTION_CAPI void mlogc_setLogLevel(MLogLevel newLogLevel, CFErrorRef* error)
{
    try {
        LoggerConfig::setLogLevel(newLogLevel);
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
}

MORPHUNTION_CAPI MLogLevel mlogc_getLogLevel()
{
    return LoggerConfig::getLogLevel();
}

MORPHUNTION_CAPI void mlogc_logToConsole(bool value, CFErrorRef* error)
{
    try {
        LoggerConfig::logToConsole(value);
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
}

MORPHUNTION_CAPI bool mlogc_registerLogger(void* context, MLoggerCallback loggerCallback, CFErrorRef* error)
{
    try {
        return LoggerConfig::registerLogger(context, loggerCallback);
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return false;
}

MORPHUNTION_CAPI bool mlogc_unregisterLogger(void* context, CFErrorRef* error)
{
    try {
        return LoggerConfig::unregisterLogger(context);
    }
    catch (const ::std::exception& e) {
        CFUtils::convert(e, error);
    }
    return false;
}
