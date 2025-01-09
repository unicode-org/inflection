/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>

namespace inflection
{
    //! Main tokenization classes.
    namespace tokenizer
    {
        class CharRange;
        class Chunk;
        class StringTokenizerBridgeProvider;
        class Token;
        class TokenChain;
        class TokenExtractor;
        class TokenIterator;
        class TokenUtil;
        class Token_Delim;
        class Token_Head;
        class Token_Tail;
        class Token_Word;
        class Tokenizer;
        class TokenizerFactory;
    } // tokenizer
} // inflection
