/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <vector>
#include <mutex>
#include <sys/utsname.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <os/log.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFPropertyList.h>
#include <CoreFoundation/CFStream.h>
#include <inflection/util/TypeConversionUtils.hpp>

namespace inflection::util {

static os_log_t getOSLog() {
    static auto log = os_log_create("com.apple.inflection", "libinflection");
    return log;
}

INFLECTION_CBEGIN
static void logOS(void* context, ILogLevel logLevel, const char16_t*, const char16_t* message) {
    auto logger = (os_log_t)context;
    std::string str;
    StringViewUtils::convert(&str, message);
    switch (logLevel) {
        case ILOG_TRACE:
            os_log_with_type(logger, OS_LOG_TYPE_DEBUG, "[TRACE] %{private}s", str.c_str());
            break;
        case ILOG_DEBUG:
            os_log_with_type(logger, OS_LOG_TYPE_DEBUG, "[DEBUG] %{private}s", str.c_str());
            break;
        case ILOG_INFO:
            os_log_with_type(logger, OS_LOG_TYPE_INFO, "[INFO] %{public}s", str.c_str());
            break;
        case ILOG_WARNING:
            os_log_with_type(logger, OS_LOG_TYPE_INFO, "[WARNING] %{public}s", str.c_str());
            break;
        case ILOG_ERROR:
            os_log_with_type(logger, OS_LOG_TYPE_ERROR, "[ERROR] %{public}s", str.c_str());
            break;
        default:
            break;
    }
}
INFLECTION_CEND

#define DEFAULT_LOGGERS {{(void*)getOSLog(), logOS}}

} // namespace inflection::util


#else
#define DEFAULT_LOGGERS
#endif // #if defined(__APPLE__) && defined(__MACH__)

namespace inflection::util {

static constexpr auto PLATFORM_SEPARATOR = u" ";

static ::std::u16string generatePlatformString()
{
    ::std::u16string result;

#if defined(__APPLE__) && defined(__MACH__)
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
                            result += TypeConversionUtils::to_u16string(value);
                        }
                        value = (CFStringRef)CFDictionaryGetValue((CFDictionaryRef)systemVersionPropertyList.value, CFSTR("ProductVersion"));
                        if (value != nullptr) {
                            result += u"-" + TypeConversionUtils::to_u16string(value);
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

static std::vector<::std::pair<void*, ILoggerCallback>>* LOGGER_CACHE()
{
    static auto LOGGER_CACHE_ = new ::std::vector<::std::pair<void*, ILoggerCallback>>(DEFAULT_LOGGERS);
    return LOGGER_CACHE_;
}

bool
Logger::registerLogger(void* context, ILoggerCallback loggerCallback) {
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

static void logToTopOfStackLogger(ILogLevel logLevel, const char16_t* component, std::u16string_view message) {
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
        logToTopOfStackLogger(ILOG_TRACE, nullptr, message);
    }
}

void Logger::debug(std::u16string_view message) {
    if (LoggerConfig::isDebugEnabled()) {
        logToTopOfStackLogger(ILOG_DEBUG, nullptr, message);
    }
}

void Logger::info(std::u16string_view message) {
    if (LoggerConfig::isInfoEnabled()) {
        logToTopOfStackLogger(ILOG_INFO, nullptr, message);
    }
}

void Logger::infoComponent(const char16_t* component, std::u16string_view message) {
    if (LoggerConfig::isInfoEnabled()) {
        logToTopOfStackLogger(ILOG_INFO, component, std::u16string(message) + PLATFORM_LOG_STRING());
    }
}

void Logger::warn(std::u16string_view message) {
    if (LoggerConfig::isWarnEnabled()) {
        logToTopOfStackLogger(ILOG_WARNING, nullptr, message);
    }
}

void Logger::error(std::u16string_view message) {
    if (LoggerConfig::isErrorEnabled()) {
        logToTopOfStackLogger(ILOG_ERROR, nullptr, message);
    }
}

} // namespace inflection::util
