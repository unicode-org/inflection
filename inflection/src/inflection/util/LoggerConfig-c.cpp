/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/LoggerConfig.h>
#include <inflection/util/Logger.hpp>
#include <inflection/util/TypeConversionUtils.hpp>

using ::inflection::util::LoggerConfig;
using ::inflection::util::TypeConversionUtils;

INFLECTION_CAPI void ilogc_setLogLevel(ILogLevel newLogLevel, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            LoggerConfig::setLogLevel(newLogLevel);
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
}

INFLECTION_CAPI ILogLevel ilogc_getLogLevel()
{
    return LoggerConfig::getLogLevel();
}

INFLECTION_CAPI void ilogc_logToConsole(bool value, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            LoggerConfig::logToConsole(value);
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
}

INFLECTION_CAPI bool ilogc_registerLogger(void* context, ILoggerCallback loggerCallback, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return LoggerConfig::registerLogger(context, loggerCallback);
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI bool ilogc_unregisterLogger(void* context, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return LoggerConfig::unregisterLogger(context);
        }
        catch (const ::std::exception& e) {
            TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}
