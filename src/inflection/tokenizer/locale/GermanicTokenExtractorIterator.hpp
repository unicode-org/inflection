/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fwd.hpp>
#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/iterator/TokenExtractorIterator.hpp>
#include <string_view>
#include <vector>

class inflection::tokenizer::locale::GermanicTokenExtractorIterator
    : public virtual ::inflection::tokenizer::iterator::TokenExtractorIterator
{
public:
    typedef ::inflection::tokenizer::iterator::TokenExtractorIterator super;
private:
    const ::inflection::tokenizer::dictionary::GermanicDecompounder& decompounder;
    inflection::tokenizer::iterator::ICUTokenExtractorIterator tokenExtractorIterator;
    std::u16string_view charSequence {  };
    std::vector<int32_t> boundaries {  };
    int32_t boundariesIdx {  };
    int32_t lastStart {  };
    int32_t minimumLenToDecompound {  };
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
    GermanicTokenExtractorIterator(const ::inflection::tokenizer::dictionary::GermanicDecompounder& decompounder, std::u16string_view str);
    ~GermanicTokenExtractorIterator() override;
private:
    GermanicTokenExtractorIterator(const GermanicTokenExtractorIterator&) = delete;
    GermanicTokenExtractorIterator& operator=(const GermanicTokenExtractorIterator&) = delete;
};
