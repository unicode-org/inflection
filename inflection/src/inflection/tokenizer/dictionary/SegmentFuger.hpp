/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <inflection/tokenizer/trie/fwd.hpp>

#include <vector>
#include <string>

class inflection::tokenizer::dictionary::SegmentFuger final
{
private:
    const DictionaryTokenizerConfig& config;
    const ::inflection::tokenizer::trie::SerializedTrie& corpus;
    int32_t maxFugenelementLength {  };
    int32_t minSegmentLength {  };
    int32_t maxReplacementFreq {  };

public:
    void detachFugenelement(Segment* word) const;

    SegmentFuger(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus);
};
