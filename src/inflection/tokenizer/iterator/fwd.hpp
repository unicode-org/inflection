/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>

namespace inflection
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
} // inflection
