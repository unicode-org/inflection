/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/util/LoggerConfig.hpp>
#include <string>
#include <string_view>

class morphuntion::util::Logger final
{
public:
    static bool registerLogger(void* context, MLoggerCallback loggerCallback);

    static void trace(std::u16string_view message);
    static void debug(std::u16string_view message);
    static void info(std::u16string_view message);
    static void infoComponent(const char16_t* component, std::u16string_view message);
    static void warn(std::u16string_view message);
    static void error(std::u16string_view message);
private:
    Logger() = delete;
};
