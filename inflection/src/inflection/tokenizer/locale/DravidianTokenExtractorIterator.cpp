/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#include <inflection/tokenizer/locale/DravidianTokenExtractorIterator.hpp>
#include <inflection/tokenizer/locale/DravidianWordAndDelimiterTokenExtractor.hpp>

namespace inflection::tokenizer::locale {

DravidianTokenExtractorIterator::DravidianTokenExtractorIterator(const DravidianWordAndDelimiterTokenExtractor& extractor, std::u16string_view str)
    : extractor(extractor)
    , tokenExtractorIterator(str.data(), int32_t(str.length()))
    , charSequence(str)
{
}

DravidianTokenExtractorIterator::~DravidianTokenExtractorIterator()
{
}

int32_t DravidianTokenExtractorIterator::getNextBoundary()
{
    if (boundariesIdx < int32_t(boundaries.size())) {
        lastStart = boundaries[boundariesIdx++];
        return lastStart;
    }

    boundaries.clear();
    boundariesIdx = 0;

    int32_t boundary;
    if ((boundary = tokenExtractorIterator.getNextBoundary()) != UBRK_DONE) {
        if (boundary - lastStart >= DravidianWordAndDelimiterTokenExtractor::MINIMUM_WORD_LENGTH) {
            extractor.decompoundWord(&boundaries, charSequence, lastStart, boundary);
            if (boundaries.size() > 1) {
                if (boundaries.back() != boundary) {
                    boundaries.emplace_back(boundary);
                }
                boundary = boundaries[boundariesIdx++];
            }
        }
        lastStart = boundary;
    }
    return boundary;
}

void DravidianTokenExtractorIterator::reset(std::u16string_view str)
{
    charSequence = str;
    lastStart = 0;
    tokenExtractorIterator.reset(str);
    boundaries.clear();
    boundariesIdx = 0;
}

} // namespace inflection::tokenizer::locale
