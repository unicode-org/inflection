/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <string>

/**
 * This class is used for finding resource files (not compiled in).
 */
class MORPHUNTION_INTERNAL_API morphuntion::util::ResourceLocator final
{
public:
    static ::std::u16string getRootForLocale(const morphuntion::util::ULocale& locale);

private:
    static const char16_t* MORPHUNTION_ROOT_DIRECTORY();

    static ::std::u16string getDirectoryFromEnvironment(const char* key, const void *dependentLibraryFunction);
};

