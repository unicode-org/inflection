/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/util/LoggerConfig.h>

/**
 * @brief Configuration for logging in Morphuntion
 */
class MORPHUNTION_CLASS_API morphuntion::util::LoggerConfig final
{
public:
    static bool isTraceEnabled();
    static bool isDebugEnabled();
    static bool isInfoEnabled();
    static bool isWarnEnabled();
    static bool isErrorEnabled();

    /**
     * Sets the desired level of logging.
     */
    static void setLogLevel(MLogLevel newLogLevel);

    /**
     * Returns the current level of logging.
     */
    static MLogLevel getLogLevel();

    /**
     * When true, the C++ std::clog stream will be used to log all logging information from Morphuntion.
     */
    static void logToConsole(bool value);

    /**
     * Registers a callback for all logging events above the current log level.
     * @param context A unique pointer for containing a context.
     * @param loggerCallback The C callback function to call
     * @return true if successfully added
     */
    static bool registerLogger(void* context, MLoggerCallback loggerCallback);

    /**
     * Unregisters a callback for all logging events
     * @param context A unique pointer for containing a context.
     * @return true if successfully removed
     */
    static bool unregisterLogger(void* context);

private:
    LoggerConfig() = delete;
};
