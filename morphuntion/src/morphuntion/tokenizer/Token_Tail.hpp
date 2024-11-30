/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/fwd.hpp>
#include <morphuntion/tokenizer/Token_Delim.hpp>

/**
 * @brief Represents the end of a TokenChain
 * @details This class represents the end of a linked-list of Tokens. It is used internally, and can be used
 * externally (when using iterators), to detect where the end of the TokenChain is. It has no other use than
 * this purpose.
 */
class MORPHUNTION_INTERNAL_API morphuntion::tokenizer::Token_Tail
    : public Token_Delim
{
public:
    typedef Token_Delim super;

protected: /* package */
    Token* newInstance(int32_t offset) const override;

protected: /* protected */
    explicit Token_Tail(int32_t utteranceLength);

private:
    friend class TokenChain;
    friend class ::morphuntion::tokenizer::Tokenizer;
    friend class ::morphuntion::tokenizer::TokenUtil;
};
