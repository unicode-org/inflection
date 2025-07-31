/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/dictionary/fwd.hpp>
#include <set>
#include <string>
#include <vector>
#include <string_view>

class inflection::tokenizer::dictionary::DictionaryTokenizerConfig
{
private: /* protected */
    int32_t maxCompoundLength { 20 };
    int32_t maxDepth { 10 };
    int32_t minCandidateLength {  };
    int32_t minSegmentLength {  };
    int32_t expectedSegmentLength { 4 };
    int32_t minFrequency { 4 };
    int32_t minFrequenciesDiff { 2 };
    int32_t maxFugenelementLength {  };
    int32_t maxReplacementFreq { 200 };
    int32_t maxCompoundFreq {  };
    int32_t minScore { 100 };
    float fallbackFreq { 0.1f };
    float lowerMinScoreRatio {  };
    float upperMinScoreRatio {  };
    int32_t minCompoundLengthForCredibility {  };
    ::std::set<::std::u16string_view> fugenelements_positive {  };
    ::std::set<::std::u16string_view> fugenelements_replaceable {  };
    ::std::vector<::std::u16string_view> replacements {  };
    ::std::vector<::std::u16string_view> fugenelements_negative {  };
    ::std::set<::std::u16string, std::less<>> fugenelements {  };

public:
    bool isFugenelement(std::u16string_view key) const;
    bool isPositiveFugenelement(std::u16string_view key) const;
    bool isReplaceableFugenelement(std::u16string_view key) const;
    const ::std::vector<::std::u16string_view>& getNegativeFugenelements() const;
    const ::std::vector<::std::u16string_view>& getReplacements() const;
    int32_t getMaxCompoundLength() const;
    int32_t getMinCandidateLength() const;
    int32_t getMinSegmentLength() const;
    int32_t getExpectedSegmentLength() const;
    int32_t getMinFrequency() const;
    int32_t getMinFrequenciesDiff() const;
    int32_t getMaxReplacementFreq() const;
    int32_t getMinScore() const;
    float getFallbackFreq() const;
    float getUpperMinScoreRatio() const;
    int32_t getMaxFugenelementLength() const;
    int32_t getMaxCompoundFreq() const;
    float getLowerMinScoreRatio() const;
    int32_t getMinCompoundLengthForCredibility() const;
    int32_t getMaxDepth() const;

protected:
    void setMinCandidateLength(int32_t minCandidateLength);
    void setMinSegmentLength(int32_t minSegmentLength);
    void setMaxCompoundFreq(int32_t maxCompoundFreq);
    void setUpperMinScoreRatio(float upperMinScoreRatio);
    void setLowerMinScoreRatio(float lowerMinScoreRatio);
    void setMinCompoundLengthForCredibility(int32_t minCredibilityLength);

public:
    DictionaryTokenizerConfig(
            const ::std::set<::std::u16string_view>& fugenelements_positive,
            const ::std::set<::std::u16string_view>& fugenelements_replaceable,
            const ::std::vector<::std::u16string_view>& replacements,
            const ::std::vector<::std::u16string_view>& fugenelements_negative
    );
};
