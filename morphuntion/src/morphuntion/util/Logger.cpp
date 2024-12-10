/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/AutoCFRelease.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/LoggerConfig.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include <vector>
#include <mutex>
#include <sys/utsname.h>

#ifndef __linux__
#include <os/log.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFPropertyList.h>
#include <CoreFoundation/CFStream.h>
#include <morphuntion/util/CFUtils.hpp>

namespace morphuntion::util {

static os_log_t getOSLog() {
    static auto log = os_log_create("com.apple.morphuntion", "libmorphuntion");
    return log;
}

MORPHUNTION_CBEGIN
static void logOS(void* context, MLogLevel logLevel, const char16_t*, const char16_t* message) {
    auto logger = (os_log_t)context;
    std::string str;
    StringViewUtils::convert(&str, message);
    switch (logLevel) {
        case MLOG_TRACE:
            os_log_with_type(logger, OS_LOG_TYPE_DEBUG, "[TRACE] %{private}s", str.c_str());
            break;
        case MLOG_DEBUG:
            os_log_with_type(logger, OS_LOG_TYPE_DEBUG, "[DEBUG] %{private}s", str.c_str());
            break;
        case MLOG_INFO:
            os_log_with_type(logger, OS_LOG_TYPE_INFO, "[INFO] %{public}s", str.c_str());
            break;
        case MLOG_WARNING:
            os_log_with_type(logger, OS_LOG_TYPE_INFO, "[WARNING] %{public}s", str.c_str());
            break;
        case MLOG_ERROR:
            os_log_with_type(logger, OS_LOG_TYPE_ERROR, "[ERROR] %{public}s", str.c_str());
            break;
        default:
            break;
    }
}
MORPHUNTION_CEND

#define DEFAULT_LOGGERS {{(void*)getOSLog(), logOS}}

} // namespace morphuntion::util


#else
#define DEFAULT_LOGGERS
#endif // #ifndef __linux__

namespace morphuntion::util {

static constexpr auto PLATFORM_SEPARATOR = u" ";

static ::std::u16string generatePlatformString()
{
    ::std::u16string result;

#if !defined(__linux__)
    // Add the product name (macOS, iOS, etc.) and product version from Apple operating systems
    AutoCFRelease<CFURLRef> systemVersionURL(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, CFSTR("/System/Library/CoreServices/SystemVersion.plist"), kCFURLPOSIXPathStyle, false));
    if (systemVersionURL) {
        AutoCFRelease<CFMutableDictionaryRef> systemVersionDict(CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
        if (systemVersionDict) {
            AutoCFRelease<CFReadStreamRef> stream(CFReadStreamCreateWithFile(kCFAllocatorDefault, systemVersionURL.value));
            if (stream) {
                if (CFReadStreamOpen(stream.value)) {
                    AutoCFRelease<CFPropertyListRef> systemVersionPropertyList(CFPropertyListCreateWithStream(kCFAllocatorDefault, stream.value, 0, kCFPropertyListImmutable, nullptr, nullptr));
                    if (CFGetTypeID(systemVersionPropertyList.value) == CFDictionaryGetTypeID()) {
                        auto value = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)systemVersionPropertyList.value, CFSTR("ProductName"));
                        if (value != nullptr) {
                            result += CFUtils::to_u16string(value);
                        }
                        value = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)systemVersionPropertyList.value, CFSTR("ProductVersion"));
                        if (value != nullptr) {
                            result += u"-" + CFUtils::to_u16string(value);
                        }
                    }
                }
                CFReadStreamClose(stream.value);
            }
        }
    }
#endif

    struct utsname sysinfo;
    if (uname(&sysinfo) == 0) {
        if (result.empty()) {
            // Either we failed to get the version information from an Apple OS or it's another OS.
            result = StringViewUtils::to_u16string(sysinfo.sysname)
                    + u"-" + StringViewUtils::to_u16string(sysinfo.release);
        }
        result += u"-" + StringViewUtils::to_u16string(sysinfo.machine);
    }
    return ::std::u16string(u"Platform=") + result;
}

static const ::std::u16string &PLATFORM_LOG_STRING()
{
    static auto PLATFORM_LOG_STRING_ = new ::std::u16string(PLATFORM_SEPARATOR + generatePlatformString());
    return *npc(PLATFORM_LOG_STRING_);
}

static std::mutex& CLASS_MUTEX() {
    static auto classMutex = new std::mutex();
    return *npc(classMutex);
}

static std::vector<::std::pair<void*, MLoggerCallback>>* LOGGER_CACHE()
{
    static auto LOGGER_CACHE_ = new ::std::vector<::std::pair<void*, MLoggerCallback>>(DEFAULT_LOGGERS);
    return LOGGER_CACHE_;
}

bool
Logger::registerLogger(void* context, MLoggerCallback loggerCallback) {
    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    auto& loggers = *npc(LOGGER_CACHE());
    if (loggerCallback == nullptr) {
        auto end = loggers.end();
        auto iter = loggers.begin();
        for (; iter != end; ++iter) {
            if (iter->first == context) {
                loggers.erase(iter);
                return true;
            }
        }
        return false;
    }
    else {
        for (const auto& entry : loggers) {
            if (entry.first == context) {
                return false;
            }
        }
        loggers.emplace_back(context, loggerCallback);
        return true;
    }
}

static void logToTopOfStackLogger(MLogLevel logLevel, const char16_t* component, std::u16string_view message) {
    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    auto& loggers = *npc(LOGGER_CACHE());
    if (!loggers.empty()) {
        // Instead of doing all the loggers, we're just doing the last one.
        auto [context, logger] = loggers.back();
        logger(context, logLevel, component, message.data());
    }
}

void Logger::trace(std::u16string_view message) {
    if (LoggerConfig::isTraceEnabled()) {
        logToTopOfStackLogger(MLOG_TRACE, nullptr, message);
    }
}

void Logger::debug(std::u16string_view message) {
    if (LoggerConfig::isDebugEnabled()) {
        logToTopOfStackLogger(MLOG_DEBUG, nullptr, message);
    }
}

void Logger::info(std::u16string_view message) {
    if (LoggerConfig::isInfoEnabled()) {
        logToTopOfStackLogger(MLOG_INFO, nullptr, message);
    }
}

void Logger::infoComponent(const char16_t* component, std::u16string_view message) {
    if (LoggerConfig::isInfoEnabled()) {
        logToTopOfStackLogger(MLOG_INFO, component, std::u16string(message) + PLATFORM_LOG_STRING());
    }
}

void Logger::warn(std::u16string_view message) {
    if (LoggerConfig::isWarnEnabled()) {
        logToTopOfStackLogger(MLOG_WARNING, nullptr, message);
    }
}

void Logger::error(std::u16string_view message) {
    if (LoggerConfig::isErrorEnabled()) {
        logToTopOfStackLogger(MLOG_ERROR, nullptr, message);
    }
}

} // namespace morphuntion::util
