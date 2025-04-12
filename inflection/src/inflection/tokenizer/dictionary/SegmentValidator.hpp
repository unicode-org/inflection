/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <inflection/tokenizer/dictionary/SegmentFuger.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include <inflection/Object.hpp>

class inflection::tokenizer::dictionary::SegmentValidator final
    : public ::inflection::Object
{
public:
    typedef ::inflection::Object super;

private:
    SegmentFuger segmentFuger;
    int32_t minFrequenciesDiff {  };
    int32_t minSegmentLength {  };
    int32_t minFrequency {  };

public:
    bool validateFirst(Segment* segment, Segment* prev, Segment* next, bool isStringTail) const;
    bool validateSecond(Segment* segment) const;

    SegmentValidator(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus);
    ~SegmentValidator() override;

};
