/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/tokenizer/Token.hpp>
#include <string_view>

/**
 * @brief Represents a delimiter within a TokenChain.
 * @details This class is used to represent delimiting characters or @ref Token "Token"s within a TokenChain. These
 * delimiting @ref Token "Token"s are commonly used to determine where words, sentences, or entire
 * @ref TokenChain "TokenChain"s begin and end. This type of Token is commonly used to represent word and sentence
 * boundaries.
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::Token_Delim
    : public Token
{
public:
    typedef Token super;

protected: /* package */
    Token* newInstance(int32_t offset) const override;

public:
    /**
     * Creates a new Token_Delim based off of the offset of the delimiter, input, and clean value.
     *
     * See Token for more information on values and clean values.
     *
     * @param begin The offset into the string the delimiter starts at
     * @param end The offset into the string the delimiter ends at
     * @param input The string that represents this delimiter
     * @param clean The clean value of the string that represents this delimiter
     */
    Token_Delim(int32_t begin, int32_t end, std::u16string_view input, std::u16string_view clean);

private:
    friend class Token;
};
