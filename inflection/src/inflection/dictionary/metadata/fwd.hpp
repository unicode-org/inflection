/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
// Forward declarations for inflection.dictionary.metadata
#pragma once

#include <inflection/api.h>
// INFLECTION_CLASS_API

namespace inflection
{
    namespace dictionary
    {
        //! Data structures that provide low-level access to the dictionary metadata.
        namespace metadata
        {
            /// @cond FORWARD_DECLARATIONS
            class CharsetConverter;
            template <typename T>
            class MarisaTrie;
            template <typename T>
            class CompressedArray;
            template <typename T>
            class MarisaTrieIterator;
            class StringArrayContainer;
            class StringContainer;
            /// @endcond
        } // metadata
    } // dictionary
} // inflection

