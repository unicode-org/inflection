/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/iterator/fwd.hpp>
#include <inflection/Object.hpp>
#include <icu4cxx/BreakIterator.hpp>

class inflection::tokenizer::iterator::TokenExtractorIterator
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;
public:

    /**
     * Return the end of the current token.
     */
    virtual int32_t getNextBoundary() = 0;

    /**
     * Reset the iterator with a new string.
     * @param str The new string to iterator over.
     */
    virtual void reset(std::u16string_view str) = 0;
public:
    TokenExtractorIterator();
    ~TokenExtractorIterator() override;
};
