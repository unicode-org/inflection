/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <inflection/tokenizer/trie/fwd.hpp>
#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/dictionary/ParsingsScorer.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <inflection/tokenizer/dictionary/SegmentValidator.hpp>
#include <vector>
#include <string>
#include <memory>
#include <string_view>

class inflection::tokenizer::dictionary::GermanicDecompounder final
{
private:
    DictionaryTokenizerConfig config;
    ::inflection::tokenizer::trie::SerializedTrie corpus;
    SegmentValidator segmentValidator;
    ParsingsScorer parsingsScorer;
    int32_t maxDepth {  };
    int32_t maxCompoundLengthMinusOne {  };
    int32_t minCandidateLength {  };
    int32_t maxCompoundFreq {  };

public: /* protected */
    void analyze(inflection::tokenizer::dictionary::Segment::SharedPtr parent, inflection::tokenizer::dictionary::Segment::SharedPtr word, int32_t offset, bool isStringTail, ::std::vector<inflection::tokenizer::dictionary::Segment::SharedPtr>* leaves) const;

private:
    inflection::tokenizer::dictionary::Segment::SharedPtr _newSegment(::std::u16string_view compound, int32_t currentOffset, int32_t compoundLength) const;
    inflection::tokenizer::dictionary::Segment::SharedPtr _newSegment(::std::u16string_view compound, int32_t offset, int32_t currentOffset, int32_t rootOffset, int32_t fuge) const;

public:
    void decompound(std::vector<int32_t>* boundaries, std::u16string_view phrase, int32_t start, int32_t length) const;
    int32_t getMinimumCandidateLength() const;

public:
    GermanicDecompounder(const DictionaryTokenizerConfig& config, const ::std::u16string& dictionaryPath);
    ~GermanicDecompounder();
private:
    GermanicDecompounder(const GermanicDecompounder& other) = delete;
    GermanicDecompounder& operator=(const GermanicDecompounder& other) = delete;
    GermanicDecompounder(GermanicDecompounder&& other) = delete;
    GermanicDecompounder const & operator=(GermanicDecompounder &&fp) = delete;
};
