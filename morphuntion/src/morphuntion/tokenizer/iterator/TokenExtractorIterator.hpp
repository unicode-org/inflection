/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/iterator/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <icu4cxx/BreakIterator.hpp>

class morphuntion::tokenizer::iterator::TokenExtractorIterator
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;
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
