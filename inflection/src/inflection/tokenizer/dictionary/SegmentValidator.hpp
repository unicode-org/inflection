/*
 * Copyright 2016-2025 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>

class inflection::tokenizer::dictionary::SegmentValidator final
{
private:
    const DictionaryTokenizerConfig& config;
    const ::inflection::tokenizer::trie::SerializedTrie& corpus;
    int32_t maxFugenelementLength {  };
    int32_t maxReplacementFreq {  };
    int32_t minFrequenciesDiff {  };
    int32_t minSegmentLength {  };
    int32_t minFrequency {  };

public:
    bool validateFirst(Segment* segment, Segment* prev, Segment* next, bool isStringTail) const;
    bool validateSecond(Segment* segment) const;

private:
    void detachFugenelement(Segment* word) const;

public:
    SegmentValidator(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus);
};
