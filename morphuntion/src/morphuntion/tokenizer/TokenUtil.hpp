/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/tokenizer/Token.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Token_Head.hpp>
#include <morphuntion/tokenizer/Token_Tail.hpp>
#include <icu4cxx/fwd.hpp>
#include <string>
#include <string_view>

class morphuntion::tokenizer::TokenUtil final
{
public:
    /**
     * @param tail The current tail
     * @param newToken The token to append
     * @return The new tail
     */
    static morphuntion::tokenizer::Token* appendToken(morphuntion::tokenizer::Token* tail, morphuntion::tokenizer::Token* newToken);

    static bool containsAllDelimitors(std::u16string_view s);
    static bool isSignificant(std::u16string_view value);
    static bool isDelimiterPriorToDash(Token* token);

private:
    TokenUtil() = delete;

private:
    static const ::icu4cxx::UnicodeSet &DELIMITER_CHARACTERS();
    static const ::icu4cxx::UnicodeSet &PRIOR_TO_DASH_DELIMITER();

public:
    static const ::icu4cxx::UnicodeSet &SIGNIFICANT_CHARACTERS();
};

inline
morphuntion::tokenizer::Token* morphuntion::tokenizer::TokenUtil::appendToken(morphuntion::tokenizer::Token* tail, morphuntion::tokenizer::Token* newToken)
{
    if (tail != nullptr) {
        tail->setNext(newToken);
        if (newToken != nullptr) {
            newToken->setPrevious(tail);
        }
    }
    return newToken;
}
