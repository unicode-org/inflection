/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fi/fwd.hpp>
#include <inflection/tokenizer/trie/fwd.hpp>
#include <string_view>
#include <string>
#include <vector>

class inflection::tokenizer::locale::fi::FiSegment final
{
private:
    const ::inflection::tokenizer::trie::SerializedTrie& corpus;
    ::std::u16string_view label {  };
    ::std::u16string_view originalCompound {  };
    int32_t breakPoint {  };
    float logProb {  };
    int32_t start {  };
    int32_t end {  };
    bool isCompound {  };
    bool found_ {  };
    bool isTail {  };

public:
    ::std::u16string_view getLabel() const;
    bool found() const;
    float getLogProb() const;
    int32_t getStart() const;
    int32_t getEnd() const;
    int32_t getLength() const;
    void addBoundaries(::std::vector<int32_t>* indexes) const;

    FiSegment(const FiSegment* parent, int32_t start, int32_t end, bool isTail);
    FiSegment(const ::inflection::tokenizer::trie::SerializedTrie& corpus, ::std::u16string_view compound, int32_t start, int32_t end, bool isTail);
};
