/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/ResourceLocator.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/resources/DataRegistrationService.hpp>
#include <CoreFoundation/CFBase.h>
#include <dlfcn.h>

namespace morphuntion::util {

#ifndef MORPHUNTION_DATA_ROOT_DIR
#define MORPHUNTION_DATA_ROOT_DIR "/usr/share"
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
                while (StringViewUtils::endsWith(libraryDirectory, u"/"));
                for (::std::u16string dirSuffix : {u"/usr/local/lib", u"/usr/lib"}) {
                    if (StringViewUtils::endsWith(libraryDirectory, dirSuffix)) {
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

const char16_t* ResourceLocator::MORPHUNTION_ROOT_DIRECTORY()
{
    static auto MORPHUNTION_ROOT_DIRECTORY_ = StringViewUtils::strdup(ResourceLocator::getDirectoryFromEnvironment("MORPHUNTION_ROOT", (const void *)&ResourceLocator::getDirectoryFromEnvironment).c_str());
    return MORPHUNTION_ROOT_DIRECTORY_;
}

::std::u16string morphuntion::util::ResourceLocator::getRootForLocale(const morphuntion::util::ULocale& locale) {
    auto dataRegistrationServicePath = resources::DataRegistrationService::getDataPathForLocale(locale);
    if (!dataRegistrationServicePath.empty()) {
        return StringViewUtils::to_u16string(dataRegistrationServicePath);
    }
    return MORPHUNTION_ROOT_DIRECTORY() + ::std::u16string(u"" MORPHUNTION_DATA_ROOT_DIR "/morphuntion");
}

} // namespace morphuntion::util
