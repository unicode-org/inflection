/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/ResourceLocator.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/resources/DataRegistrationService.hpp>
#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#endif
#include <dlfcn.h>

namespace inflection::util {

#ifndef INFLECTION_DATA_ROOT_DIR
#define INFLECTION_DATA_ROOT_DIR "/usr/share"
#endif

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
        // Make a guess as if we're in a production style environment instead of a development environment.
        Dl_info dl_info;
        dladdr(dependentLibraryFunction, &dl_info);
        auto libraryFilename = dl_info.dli_fname;
        if (libraryFilename != nullptr && libraryFilename[0] != 0) {
            auto libraryDirectory(StringViewUtils::to_u16string(libraryFilename));
            auto libraryDirectoryIdx = libraryDirectory.find_last_of(u'/');
            if (libraryDirectoryIdx != ::std::u16string::npos) {
                do {
                    libraryDirectory.resize(libraryDirectoryIdx--);
                }
                while (libraryDirectory.ends_with(u"/"));
                for (::std::u16string dirSuffix : {u"/usr/local/lib", u"/usr/lib"}) {
                    if (libraryDirectory.ends_with(dirSuffix)) {
                        // Assume that we're sharing one big happy root.
                        retVal.assign(libraryDirectory, 0, libraryDirectory.length() - dirSuffix.length());
                        break;
                    }
                }
            }
        }
    }
    return retVal;
}

const char16_t* ResourceLocator::INFLECTION_ROOT_DIRECTORY()
{
    static auto INFLECTION_ROOT_DIRECTORY_ = StringViewUtils::strdup(ResourceLocator::getDirectoryFromEnvironment("INFLECTION_ROOT", (const void *)&ResourceLocator::getDirectoryFromEnvironment).c_str());
    return INFLECTION_ROOT_DIRECTORY_;
}

::std::u16string inflection::util::ResourceLocator::getRootForLocale(const inflection::util::ULocale& locale) {
    auto dataRegistrationServicePath = resources::DataRegistrationService::getDataPathForLocale(locale);
    if (!dataRegistrationServicePath.empty()) {
        return StringViewUtils::to_u16string(dataRegistrationServicePath);
    }
    return INFLECTION_ROOT_DIRECTORY() + ::std::u16string(u"" INFLECTION_DATA_ROOT_DIR "/inflection");
}

} // namespace inflection::util
