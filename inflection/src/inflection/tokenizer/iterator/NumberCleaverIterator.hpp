/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/iterator/fwd.hpp>
#include <inflection/tokenizer/iterator/TokenExtractorIterator.hpp>
#include <icu4cxx/BreakIterator.hpp>

class inflection::tokenizer::iterator::NumberCleaverIterator
        : public virtual ::inflection::tokenizer::iterator::TokenExtractorIterator
{
public:
    typedef ::inflection::tokenizer::iterator::TokenExtractorIterator super;
private:
    std::u16string_view str;
    TokenExtractorIterator& tokenIterator;
    int32_t start { 0 };
    int32_t split { UBRK_DONE };
    int32_t end { 0 };
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
    NumberCleaverIterator(std::u16string_view str, TokenExtractorIterator& tokenIterator);
    ~NumberCleaverIterator() override;
};