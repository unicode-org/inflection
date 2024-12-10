/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/locale/SemiticTokenExtractorIterator.hpp>

#include <morphuntion/tokenizer/locale/SemiticWordAndDelimiterTokenExtractor.hpp>

namespace morphuntion::tokenizer::locale {

SemiticTokenExtractorIterator::SemiticTokenExtractorIterator(const ::morphuntion::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor& decompounder, std::u16string_view str)
    : decompounder(decompounder)
    , tokenExtractorIterator(str.data(), int32_t(str.length()))
    , charSequence(str)
{
}

SemiticTokenExtractorIterator::~SemiticTokenExtractorIterator()
{
}

int32_t SemiticTokenExtractorIterator::getNextBoundary()
{
    if (boundariesIdx < (int32_t)boundaries.size()) {
        lastStart = boundaries[boundariesIdx++];
        return lastStart;
    }
    boundaries.clear();
    boundariesIdx = 0;
    int32_t boundary;
    if ((boundary = tokenExtractorIterator.getNextBoundary()) != UBRK_DONE) {
        if (boundary - lastStart >= SemiticWordAndDelimiterTokenExtractor::MINIMUM_WORD_LENGTH) {
            decompounder.decompoundWord(&boundaries, charSequence, lastStart, boundary);
            if (boundaries.size() > 1) {
                if (boundaries.back() != boundary) {
                    // Include the word with the 's suffix when needed or just include the whole thing if it couldn't be decompounded.
                    boundaries.emplace_back(boundary);
                }
                boundary = boundaries[boundariesIdx++];
            }
        }
        lastStart = boundary;
    }
    return boundary;
}

void SemiticTokenExtractorIterator::reset(std::u16string_view str)
{
    charSequence = str;
    lastStart = 0;
    tokenExtractorIterator.reset(str);
    boundaries.clear();
    boundariesIdx = 0;
}

} // namespace morphuntion::tokenizer
