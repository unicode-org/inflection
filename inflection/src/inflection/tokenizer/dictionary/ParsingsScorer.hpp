/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <vector>
#include <cstdint>
#include <string_view>

class inflection::tokenizer::dictionary::ParsingsScorer final
{

private:
    int32_t minScore {  };
    int32_t expectedSegmentLength {  };
    float lowerMinScoreRatio {  };
    float upperMinScoreRatio {  };
    int32_t minCompoundLengthForCredibility {  };

public:
    void getBestOffsets(std::vector<int32_t>* boundaries, int32_t offset, ::std::vector<inflection::tokenizer::dictionary::Segment::SharedPtr>* leafs) const;

private:
    Segment::SharedPtr _getBestLeaf(::std::vector<::inflection::tokenizer::dictionary::Segment::SharedPtr>* leafs) const;
    int32_t _prune(const ::std::vector<::inflection::tokenizer::dictionary::Segment::SharedPtr>& leafs) const;

public:
    explicit ParsingsScorer(const DictionaryTokenizerConfig& config);
};
