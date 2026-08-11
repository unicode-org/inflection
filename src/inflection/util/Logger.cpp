/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#include <inflection/util/AutoCRelease.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <vector>
#include <mutex>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif
#if __has_include(<sys/utsname.h>)
#include <sys/utsname.h>
#define HAS_UTSNAME
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <os/log.h>

namespace inflection::util {

static os_log_t getOSLog() {
    static auto log = os_log_create("org.unicode.inflection", "libinflection");
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

#ifdef _WIN32
    OSVERSIONINFOW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    using RtlGetVersionFunc = LONG(WINAPI*)(OSVERSIONINFOW*);
    auto ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll != nullptr) {
        auto rtlGetVersion = reinterpret_cast<RtlGetVersionFunc>(GetProcAddress(ntdll, "RtlGetVersion"));
        if (rtlGetVersion != nullptr && rtlGetVersion(&osvi) == 0) {
            result = u"Windows-"
                + StringViewUtils::to_u16string(std::to_string(osvi.dwMajorVersion))
                + u"." + StringViewUtils::to_u16string(std::to_string(osvi.dwMinorVersion))
                + u"." + StringViewUtils::to_u16string(std::to_string(osvi.dwBuildNumber));
        }
    }
    SYSTEM_INFO si = {};
    GetSystemInfo(&si);
    const char* arch = "Unknown";
    switch (si.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64: arch = "x86_64"; break;
        case PROCESSOR_ARCHITECTURE_ARM64: arch = "arm64"; break;
        default: break;
    }
    result += u"-" + StringViewUtils::to_u16string(arch);
#endif
#ifdef HAS_UTSNAME
    struct utsname sysinfo;
    if (uname(&sysinfo) == 0) {
        if (result.empty()) {
            result = StringViewUtils::to_u16string(sysinfo.sysname)
                    + u"-" + StringViewUtils::to_u16string(sysinfo.release);
        }
        result += u"-" + StringViewUtils::to_u16string(sysinfo.machine);
    }
#elif !defined(_WIN32)
#warning "This platform is unknown. No logging capability of the platform type is possible."
    result = u"?";
#endif

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
