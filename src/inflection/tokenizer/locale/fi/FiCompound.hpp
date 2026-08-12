/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fi/fwd.hpp>
#include <inflection/tokenizer/trie/fwd.hpp>
#include <inflection/Object.hpp>
#include <string>
#include <vector>

class inflection::tokenizer::locale::fi::FiCompound final
{
public: /* package */
    FiSegment* segment {  };
    FiSuffixSplit* suffixSplit {  };
    float logProb {  };
    bool found {  };

public:
    float getLogProb() const;
    void addBoundaries(int32_t stackDepthLeft, ::std::vector<int32_t>* max_indexes);

    explicit FiCompound(FiSuffixSplit* suffixSplit);
};
