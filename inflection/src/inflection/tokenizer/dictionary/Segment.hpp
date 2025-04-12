/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <inflection/tokenizer/trie/fwd.hpp>
#include <inflection/Object.hpp>
#include <string>
#include <memory>
#include <string_view>

class inflection::tokenizer::dictionary::Segment final
{
public:
    typedef ::std::shared_ptr<Segment> SharedPtr;

private:
    ::std::u16string_view compound {  };
    int32_t start {  };
    int32_t end {  };
    int32_t rootStart {  };
    int32_t fugeLen { -1 };
    ::std::u16string_view root {  };
    float freq {  };
    bool isSegment_ {  };
    bool isNoAtomic_ {  };
    bool isNoHead_ {  };
    bool isNoCompound_ {  };
    bool isEnglish_ {  };
    Segment::SharedPtr parent {  };
    float freqProduct { 1 };
    int32_t depth {  };
    int32_t splits {  };
    int32_t lengthProduct { 1 };
    float score_ {  };
    float credibility {  };
    int32_t expectedSegmentLength {  };

private:
    void _parseData(int32_t data);

public:
    void setRootAndFuge(std::u16string_view root, int32_t fugeLen, int32_t data);
    void tie(Segment::SharedPtr parent);
    float getGeometricMeanLength() const;
    int32_t getCompoundLength() const;
    float getArithmeticMeanLength() const;
    void score();
    ::std::u16string_view getCompound() const;
    int32_t getOffset() const;
    int32_t getRootOffset() const;
    float getFreq() const;
    bool isSegment() const;
    bool isNoAtomic() const;
    bool isNoHead() const;
    bool isNoCompound() const;
    bool isEnglish() const;
    int32_t getLength() const;
    float getFreqProduct() const;
    int32_t getDepth() const;
    Segment::SharedPtr getParent() const;
    float getScore() const;
    ::std::u16string_view getString() const;
    int32_t getFugeLen() const;
    int32_t getLengthProduct() const;
    float getCredibility() const;
    int32_t getSplits() const;
    
    bool operator==(const Segment& other) const = delete;

    Segment(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus, ::std::u16string_view compound, int32_t start, int32_t end);
    Segment(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus, ::std::u16string_view compound, int32_t start, int32_t end, int32_t rootStart, int32_t fugeLen);
    Segment(const Segment& other);
    ~Segment();
private:
    Segment& operator=(const Segment& other) = delete;
};
