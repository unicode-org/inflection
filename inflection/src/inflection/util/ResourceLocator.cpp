/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/resources/DataRegistrationService.hpp>
#include <inflection/npc.hpp>
#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#endif
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <cstdlib>

namespace inflection::util {

#ifndef INFLECTION_DATA_ROOT_DIR
#ifdef _WIN32
#define INFLECTION_DATA_ROOT_DIR "share"
#else
#define INFLECTION_DATA_ROOT_DIR "/usr/share"
#endif
#endif
#define U16_IMPL(x) u ## x
#define U16(x) U16_IMPL(x)

::std::u16string ResourceLocator::getDirectoryFromEnvironment(const char* key, const void *dependentLibraryFunction)
{
    ::std::u16string retVal;
    auto directory = getenv(key);
    if (directory != nullptr && directory[0] != 0) {
        retVal = StringViewUtils::to_u16string(directory);
    }
#if defined(TARGET_OS_SIMULATOR) && TARGET_OS_SIMULATOR
    else if ((directory = getenv("SIMULATOR_ROOT")) != nullptr && directory[0] != 0) {
        retVal = StringViewUtils::to_u16string(directory);
    }
#endif
    else if (directory == nullptr) {
        std::filesystem::path libraryDir;
#ifdef _WIN32
        HMODULE hModule = nullptr;
        if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                               reinterpret_cast<LPCWSTR>(dependentLibraryFunction), &hModule) && hModule != nullptr) {
            wchar_t modulePath[MAX_PATH];
            DWORD len = GetModuleFileNameW(hModule, modulePath, MAX_PATH);
            if (len > 0 && len < MAX_PATH) {
                libraryDir = std::filesystem::path(std::wstring(modulePath, len)).parent_path();
            }
        }
#else
        Dl_info dl_info;
        dladdr(dependentLibraryFunction, &dl_info);
        if (dl_info.dli_fname != nullptr && dl_info.dli_fname[0] != 0) {
            libraryDir = std::filesystem::path(dl_info.dli_fname).parent_path();
        }
#endif

        if (!libraryDir.empty()) {
            auto lastComponent = libraryDir.filename();
            if (lastComponent == "lib") {
#ifdef _WIN32
                retVal = libraryDir.parent_path().u16string();
#else
                auto parent = libraryDir.parent_path();
                if (parent.filename() == "local" && parent.parent_path().filename() == "usr") {
                    retVal = parent.parent_path().parent_path().u16string();
                } else if (parent.filename() == "usr") {
                    retVal = parent.parent_path().u16string();
                }
#endif
            }
        }
    }
    return retVal;
}

const ::std::u16string& ResourceLocator::INFLECTION_ROOT_DIRECTORY()
{
    static const auto INFLECTION_ROOT_DIRECTORY_ = new ::std::u16string(ResourceLocator::getDirectoryFromEnvironment("INFLECTION_ROOT", (const void *)&ResourceLocator::getDirectoryFromEnvironment));
    return *npc(INFLECTION_ROOT_DIRECTORY_);
}

::std::u16string inflection::util::ResourceLocator::getRootForLocale(const inflection::util::ULocale& locale) {
    auto dataRegistrationServicePath = resources::DataRegistrationService::getDataPathForLocale(locale);
    if (!dataRegistrationServicePath.empty()) {
        return StringViewUtils::to_u16string(dataRegistrationServicePath);
    }
    return INFLECTION_ROOT_DIRECTORY() + u"/" + U16(INFLECTION_DATA_ROOT_DIR) + u"/inflection";
}

} // namespace inflection::util
