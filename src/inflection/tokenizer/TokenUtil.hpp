/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/tokenizer/Token.hpp>
#include <inflection/tokenizer/TokenChain.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <icu4cxx/fwd.hpp>
#include <string>
#include <string_view>

class inflection::tokenizer::TokenUtil final
{
public:
    /**
     * @param tail The current tail
     * @param newToken The token to append
     * @return The new tail
     */
    static inflection::tokenizer::Token* appendToken(inflection::tokenizer::Token* tail, inflection::tokenizer::Token* newToken);

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
inflection::tokenizer::Token* inflection::tokenizer::TokenUtil::appendToken(inflection::tokenizer::Token* tail, inflection::tokenizer::Token* newToken)
{
    if (tail != nullptr) {
        tail->setNext(newToken);
        if (newToken != nullptr) {
            newToken->setPrevious(tail);
        }
    }
    return newToken;
}
