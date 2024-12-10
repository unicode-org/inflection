/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/api.h>
#include <CoreFoundation/CFError.h>

/**
 * These are the available levels of logging for Morphuntion.
 */
typedef CF_ENUM(CFIndex, MLogLevel) {
    MLOG_TRACE,
    MLOG_DEBUG,
    MLOG_INFO,
    MLOG_WARNING,
    MLOG_ERROR,
    MLOG_DISABLED
};

MORPHUNTION_CBEGIN
/**
 * The callback to consume logging information.
 * @param context This is a user defined context provided when a logger is registered. The pointer should be cast
 * to the underlying context.
 * @param logLevel The log level. It should not be lower the current log level.
 * @param category This argument may be null. It conveys the part of Morphuntion that logged this event, like tokenization,
 * normalization, grammar synthesis and so forth.
 * @param message A fully formatted message. It's never null.
 */
typedef void (*MLoggerCallback)(void* context, MLogLevel logLevel, const char16_t* category, const char16_t* message);
MORPHUNTION_CEND

/**
 * Sets the desired level of logging.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI void mlogc_setLogLevel(MLogLevel newLogLevel, CFErrorRef* error);

/**
 * Returns the current level of logging.
 */
MORPHUNTION_CAPI MLogLevel mlogc_getLogLevel();

/**
 * When true, the C++ std::clog stream will be used to log all logging information from Morphuntion.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI void mlogc_logToConsole(bool value, CFErrorRef* error);

/**
 * Registers a callback for all logging events above the current log level.
 * @param context A unique pointer for containing a context.
 * @param loggerCallback The C callback function to call
 * @param error The provided error reference is allocated when the function failed.
 * @return true if sucessfully added
 */
MORPHUNTION_CAPI bool mlogc_registerLogger(void* context, MLoggerCallback loggerCallback, CFErrorRef* error);

/**
 * Unregisters a callback for all logging events
 * @param context A unique pointer for containing a context.
 * @param error The provided error reference is allocated when the function failed.
 * @return true if sucessfully removed
 */
MORPHUNTION_CAPI bool mlogc_unregisterLogger(void* context, CFErrorRef* error);
