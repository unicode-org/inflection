/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#pragma once

#include <inflection/tokenizer/locale/fwd.hpp>
#include <inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <inflection/tokenizer/iterator/TokenExtractorIterator.hpp>
#include <string_view>
#include <vector>

namespace inflection::tokenizer::locale {

class DravidianWordAndDelimiterTokenExtractor;

class DravidianTokenExtractorIterator {
public:
    DravidianTokenExtractorIterator(const DravidianWordAndDelimiterTokenExtractor& extractor, std::u16string_view str);
    ~DravidianTokenExtractorIterator();

    int32_t getNextBoundary();
    void reset(std::u16string_view str);

private:
    const DravidianWordAndDelimiterTokenExtractor& extractor;
    TokenExtractorIterator tokenExtractorIterator;
    std::u16string_view charSequence;

    int32_t lastStart = 0;
    std::vector<int32_t> boundaries;
    int32_t boundariesIdx = 0;
};

} // namespace inflection::tokenizer::locale
