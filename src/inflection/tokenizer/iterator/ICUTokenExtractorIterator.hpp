/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include "inflection/tokenizer/iterator/fwd.hpp"
#include "inflection/tokenizer/iterator/TokenExtractorIterator.hpp"
#include "icu4cxx/BreakIterator.hpp"
#include <string_view>

class inflection::tokenizer::iterator::ICUTokenExtractorIterator
    : public virtual ::inflection::tokenizer::iterator::TokenExtractorIterator
{
public:
    typedef ::inflection::tokenizer::iterator::TokenExtractorIterator super;
private:
    icu4cxx::BreakIterator breakIterator;
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
    ICUTokenExtractorIterator(const char16_t *str, int32_t strLen);
    ~ICUTokenExtractorIterator() override;
private:
    ICUTokenExtractorIterator(const ICUTokenExtractorIterator&) = delete;
    ICUTokenExtractorIterator& operator=(const ICUTokenExtractorIterator&) = delete;
};
