/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fwd.hpp>
#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/iterator/TokenExtractorIterator.hpp>
#include <string_view>
#include <vector>

class inflection::tokenizer::locale::SemiticTokenExtractorIterator
    : public virtual ::inflection::tokenizer::iterator::TokenExtractorIterator
{
public:
    typedef ::inflection::tokenizer::iterator::TokenExtractorIterator super;
private:
    const ::inflection::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor& decompounder;
    inflection::tokenizer::iterator::ICUTokenExtractorIterator tokenExtractorIterator;
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
    SemiticTokenExtractorIterator(const ::inflection::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor& decompounder, std::u16string_view str);
    ~SemiticTokenExtractorIterator() override;
private:
    SemiticTokenExtractorIterator(const SemiticTokenExtractorIterator&) = delete;
    SemiticTokenExtractorIterator& operator=(const SemiticTokenExtractorIterator&) = delete;
};
