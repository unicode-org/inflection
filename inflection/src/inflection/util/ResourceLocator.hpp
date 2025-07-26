/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <string>

/**
 * This class is used for finding resource files (not compiled in).
 */
class inflection::util::ResourceLocator final
{
public:
    static ::std::u16string getRootForLocale(const inflection::util::ULocale& locale);

private:
    static const char16_t* INFLECTION_ROOT_DIRECTORY();

    static ::std::u16string getDirectoryFromEnvironment(const char* key, const void *dependentLibraryFunction);
};

