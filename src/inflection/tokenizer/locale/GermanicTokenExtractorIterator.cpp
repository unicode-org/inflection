/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/GermanicTokenExtractorIterator.hpp>

#include <inflection/tokenizer/dictionary/GermanicDecompounder.hpp>

namespace inflection::tokenizer::locale {

GermanicTokenExtractorIterator::GermanicTokenExtractorIterator(const ::inflection::tokenizer::dictionary::GermanicDecompounder& decompounder, std::u16string_view str)
    : decompounder(decompounder)
    , tokenExtractorIterator(str.data(), int32_t(str.length()))
    , charSequence(str)
    , minimumLenToDecompound(decompounder.getMinimumCandidateLength())
{
}

GermanicTokenExtractorIterator::~GermanicTokenExtractorIterator()
{
}

int32_t GermanicTokenExtractorIterator::getNextBoundary()
{
    if (boundariesIdx < (int32_t)boundaries.size()) {
        return boundaries[boundariesIdx++];
    }
    boundaries.clear();
    boundariesIdx = 0;
    int32_t boundary;
    if ((boundary = tokenExtractorIterator.getNextBoundary()) != UBRK_DONE) {
        if (boundary - lastStart >= minimumLenToDecompound) {
            auto suffixlessBoundary = boundary;
            if (charSequence[suffixlessBoundary - 1] == u's') {
                auto secondToLast = charSequence[suffixlessBoundary - 2];
                if (secondToLast == u'\u2019' || secondToLast == u'\'') {
                    suffixlessBoundary -= 2;
                }
            }
            decompounder.decompound(&boundaries, charSequence, lastStart, suffixlessBoundary - lastStart);
            if (!boundaries.empty()) {
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

void GermanicTokenExtractorIterator::reset(std::u16string_view str)
{
    charSequence = str;
    lastStart = 0;
    tokenExtractorIterator.reset(str);
    boundaries.clear();
    boundariesIdx = 0;
}

} // namespace inflection::tokenizer
