/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/fwd.hpp>
#include <inflection/tokenizer/Token_Delim.hpp>

/**
 * @brief Represents the end of a TokenChain
 * @details This class represents the end of a linked-list of Tokens. It is used internally, and can be used
 * externally (when using iterators), to detect where the end of the TokenChain is. It has no other use than
 * this purpose.
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::Token_Tail
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
    friend class ::inflection::tokenizer::Tokenizer;
    friend class ::inflection::tokenizer::TokenUtil;
};
