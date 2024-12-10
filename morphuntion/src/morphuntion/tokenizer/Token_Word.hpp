/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/tokenizer/Token.hpp>
#include <string_view>

/**
 * @brief Represents a single morpheme.
 * @details Represents a free morpheme, even if it was previously bounded.
 */
class MORPHUNTION_INTERNAL_API morphuntion::tokenizer::Token_Word
    : public Token
{
private:
    typedef Token super;

protected: /* package */
    Token* newInstance(int32_t offset) const override;

public:
    /**
     * Creates a new Token_Word based off of the offset of the delimiter, input, clean value, and significance.
     *
     * See Token for more information on values, clean values, and significance.
     *
     * @param begin The offset into the string the Token starts from.
     * @param end The offset into the string the Token ends at.
     * @param input The raw value that this Token represents.
     * @param clean The clean value that this Token represents.
     * @param significant True if this Token_Word is lexically significant.
     */
    Token_Word(int32_t begin, int32_t end, std::u16string_view input, std::u16string_view clean, bool significant);
};
