/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
// Forward declarations for morphuntion.dictionary
#pragma once

#include <morphuntion/api.h>
// MORPHUNTION_CLASS_API

namespace morphuntion
{
    //! Access to Morphuntion's low-level binary dictionaries (metadata and inflectional).
    namespace dictionary
    {
        /// @cond FORWARD_DECLARATIONS
        class DictionaryKeyIterator;
        class DictionaryMetaData;
        class DictionaryMetaData_MMappedDictionary;
        class Inflector;
        class Inflector_Inflection;
        class Inflector_InflectionPattern;
        class Inflector_MMappedDictionary;
        class PhraseProperties;
        /// @endcond
    } // dictionary
} // morphuntion
