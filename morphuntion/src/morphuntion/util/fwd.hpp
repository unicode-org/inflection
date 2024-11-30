/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
// Forward declarations for morphuntion.util
#pragma once

#include <morphuntion/api.h>
// MORPHUNTION_CLASS_API

namespace morphuntion
{
    //! Various utilities to help with logging, locales, strings, unicode characters, iterators, etc.
    namespace util
    {
        /// @cond FORWARD_DECLARATIONS
        class AutoFileDescriptor;
        template <class T, auto Destroy>
        class AutoCRelease;
        class CFUtils;
        class DelimitedStringIterator;
        template <typename Function>
        class Finally;
        class LocaleUtils;
        class Logger;
        class LoggerConfig;
        class MemoryMappedFile;
        class ResourceLocator;
        class StringUtils;
        class StringViewUtils;
        class ULocale;
        class UnicodeSetUtils;
        class Validate;
        class VersionUtils;
        /// @endcond
    } // util
} // morphuntion
