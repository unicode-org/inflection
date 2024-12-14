/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/tokenizer/Token.hpp>

/**
 * @brief Provides C++ iterator mechanics for TokenChain and Chunk.
 * @details Use TokenIterator to iterate over TokenChain or Chunk when using the C++ API. This class provides the same
 * STL-iterator-like mechanics that are used in classes like <code>std::vector</code> or <code>std::list</code>.
 *
 * An example of usage would be:<br/>
 * @code
 * ::std::unique_ptr<::morphuntion::tokenizer::TokenChain> tokenChain(tokenizer->createTokenChain(word));
 * std::vector<std::u16string> words;
 * for (const auto& token : *tokenChain) {
 *     if (dynamic_cast<const ::morphuntion::Token_Word*>(&token) != nullptr) {
 *         words.emplace_back(token.getValue());
 *     }
 * }
 * @endcode
 */
class MORPHUNTION_INTERNAL_API morphuntion::tokenizer::TokenIterator
{
public:
    explicit TokenIterator(const Token* head, const Token* end);

    const Token*     operator->() const;
    const Token&     operator*() const;
    const Token*     get() const;

    TokenIterator&   operator++();
    TokenIterator    operator++(int postIncrement);
    TokenIterator&   operator--();
    TokenIterator    operator--(int postDecrement);

    bool operator==(const TokenIterator& rhs) const;
    bool operator!=(const TokenIterator& rhs) const;

private:
    const Token* token;
    const Token* end;
};