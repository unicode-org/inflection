/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/util/LoggerConfig.h>

/**
 * @brief Configuration for logging in Inflection
 */
class INFLECTION_CLASS_API inflection::util::LoggerConfig final
{
public:
    /**
     * Is trace logging currently enabled?
     */
    static bool isTraceEnabled();
    /**
     * Is debug logging currently enabled?
     */
    static bool isDebugEnabled();
    /**
     * Is info logging currently enabled?
     */
    static bool isInfoEnabled();
    /**
     * Is warn logging currently enabled?
     */
    static bool isWarnEnabled();
    /**
     * Is error logging currently enabled?
     */
    static bool isErrorEnabled();

    /**
     * Sets the desired level of logging.
     */
    static void setLogLevel(ILogLevel newLogLevel);

    /**
     * Returns the current level of logging.
     */
    static ILogLevel getLogLevel();

    /**
     * When true, the C++ std::clog stream will be used to log all logging information from Inflection.
     */
    static void logToConsole(bool value);

    /**
     * Registers a callback for all logging events above the current log level.
     * @param context A unique pointer for containing a context.
     * @param loggerCallback The C callback function to call
     * @return true if successfully added
     */
    static bool registerLogger(void* context, ILoggerCallback loggerCallback);

    /**
     * Unregisters a callback for all logging events
     * @param context A unique pointer for containing a context.
     * @return true if successfully removed
     */
    static bool unregisterLogger(void* context);

private:
    LoggerConfig() = delete;
};
