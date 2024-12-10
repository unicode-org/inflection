/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/api.h>

namespace morphuntion
{
    namespace tokenizer
    {
        //! Tokenization iterator classes.
        namespace iterator
        {
            class ICUTokenExtractorIterator;
            class NumberCleaverIterator;
            class RegexSplitIterator;
            class TokenExtractorIterator;
        } // iterator
    } // tokenizer
} // morphuntion
