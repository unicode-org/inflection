/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/Token_Delim.hpp>

/**
 * @brief Represents the start of a TokenChain
 * @details This class represents the start of a linked-list of Tokens. It is used internally, and can be used
 * externally (when using iterators), to detect where the beginning of the TokenChain begins. It has no other use than
 * this purpose.
 */
class MORPHUNTION_INTERNAL_API morphuntion::tokenizer::Token_Head
    : public Token_Delim
{
public:
    typedef Token_Delim super;

protected: /* package */
    Token* newInstance(int32_t offset) const override;

protected: /* protected */
    Token_Head();
    explicit Token_Head(int32_t start);

private:
    friend class TokenChain;
    friend class ::morphuntion::tokenizer::Tokenizer;
    friend class ::morphuntion::tokenizer::TokenUtil;
};
