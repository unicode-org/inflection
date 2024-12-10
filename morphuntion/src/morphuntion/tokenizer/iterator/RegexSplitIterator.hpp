/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/iterator/fwd.hpp>
#include <morphuntion/tokenizer/iterator/TokenExtractorIterator.hpp>
#include <morphuntion/tokenizer/TokenExtractor.hpp>
#include <icu4cxx/RegularExpression.hpp>
#include <string>
#include <string_view>

class morphuntion::tokenizer::iterator::RegexSplitIterator final
    : public virtual ::morphuntion::tokenizer::iterator::TokenExtractorIterator
{
public:
    typedef ::morphuntion::tokenizer::iterator::TokenExtractorIterator super;
private:
    const morphuntion::tokenizer::TokenExtractor& tokenExtractor;
    std::u16string_view str;
    TokenExtractorIterator& tokenIterator;
    ::std::u16string lowercase {  };
    ::icu4cxx::RegularExpression matcher { nullptr, false };
    UText text UTEXT_INITIALIZER;
    int32_t start { UBRK_DONE };
    int32_t previousBreak { 0 };
    int32_t end { 0 };
    int32_t group { -1 };
    int32_t groupCount { -1 };

    int32_t getNextRegexBoundary();
public:

    /**
     * Return the end of the current token.
     */
    int32_t getNextBoundary() override;

    /**
     * Reset the iterator with a new string.
     * @param str The new string to iterator over.
     */
    void reset(std::u16string_view str) override;

public:
    RegexSplitIterator(const TokenExtractor& tokenExtractor, std::u16string_view str, TokenExtractorIterator& tokenIterator);
    ~RegexSplitIterator() override;
};
