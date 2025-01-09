/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
// Forward declarations for inflection.dictionary
#pragma once

#include <inflection/api.h>
// INFLECTION_CLASS_API

namespace inflection
{
    //! Access to Inflection's low-level binary dictionaries (metadata and inflectional).
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
} // inflection
