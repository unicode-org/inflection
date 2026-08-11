/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/Token_Delim.hpp>

/**
 * @brief Represents the start of a TokenChain
 * @details This class represents the start of a linked-list of Tokens. It is used internally, and can be used
 * externally (when using iterators), to detect where the beginning of the TokenChain begins. It has no other use than
 * this purpose.
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::Token_Head
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
    friend class ::inflection::tokenizer::Tokenizer;
    friend class ::inflection::tokenizer::TokenUtil;
};
