/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>
#include <unicode/utypes.h>

/**
 * These are the available levels of logging for Inflection.
 */
INFLECTION_ENUM(int8_t, ILogLevel) {
    ILOG_TRACE,
    ILOG_DEBUG,
    ILOG_INFO,
    ILOG_WARNING,
    ILOG_ERROR,
    ILOG_DISABLED
};

INFLECTION_CBEGIN
/**
 * The callback to consume logging information.
 * @param context This is a user defined context provided when a logger is registered. The pointer should be cast
 * to the underlying context.
 * @param logLevel The log level. It should not be lower the current log level.
 * @param category This argument may be null. It conveys the part of Inflection that logged this event, like tokenization,
 * normalization, grammar synthesis and so forth.
 * @param message A fully formatted message. It's never null.
 */
typedef void (*ILoggerCallback)(void* context, ILogLevel logLevel, const char16_t* category, const char16_t* message);
INFLECTION_CEND

/**
 * Sets the desired level of logging.
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI void ilogc_setLogLevel(ILogLevel newLogLevel, UErrorCode* status);

/**
 * Returns the current level of logging.
 */
INFLECTION_CAPI ILogLevel ilogc_getLogLevel();

/**
 * When true, the C++ std::clog stream will be used to log all logging information from Inflection.
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI void ilogc_logToConsole(bool value, UErrorCode* status);

/**
 * Registers a callback for all logging events above the current log level.
 * @param context A unique pointer for containing a context.
 * @param loggerCallback The C callback function to call
 * @param error The provided error reference is allocated when the function failed.
 * @return true if sucessfully added
 */
INFLECTION_CAPI bool ilogc_registerLogger(void* context, ILoggerCallback loggerCallback, UErrorCode* status);

/**
 * Unregisters a callback for all logging events
 * @param context A unique pointer for containing a context.
 * @param error The provided error reference is allocated when the function failed.
 * @return true if sucessfully removed
 */
INFLECTION_CAPI bool ilogc_unregisterLogger(void* context, UErrorCode* status);
