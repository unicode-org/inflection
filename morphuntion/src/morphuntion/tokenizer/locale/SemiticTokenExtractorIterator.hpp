/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/locale/fwd.hpp>
#include <morphuntion/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <morphuntion/tokenizer/iterator/TokenExtractorIterator.hpp>
#include <string_view>
#include <vector>

class morphuntion::tokenizer::locale::SemiticTokenExtractorIterator
    : public virtual ::morphuntion::tokenizer::iterator::TokenExtractorIterator
{
public:
    typedef ::morphuntion::tokenizer::iterator::TokenExtractorIterator super;
private:
    const ::morphuntion::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor& decompounder;
    morphuntion::tokenizer::iterator::ICUTokenExtractorIterator tokenExtractorIterator;
    std::u16string_view charSequence {  };
    std::vector<int32_t> boundaries {  };
    int32_t boundariesIdx {  };
    int32_t lastStart {  };
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
    SemiticTokenExtractorIterator(const ::morphuntion::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor& decompounder, std::u16string_view str);
    ~SemiticTokenExtractorIterator() override;
private:
    SemiticTokenExtractorIterator(const SemiticTokenExtractorIterator&) = delete;
    SemiticTokenExtractorIterator& operator=(const SemiticTokenExtractorIterator&) = delete;
};
