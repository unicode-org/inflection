/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>

#include <algorithm>

namespace inflection::tokenizer::dictionary {

DictionaryTokenizerConfig::DictionaryTokenizerConfig(
        const ::std::set<::std::u16string_view>& fugenelements_positive,
        const ::std::set<::std::u16string_view>& fugenelements_replaceable,
        const ::std::vector<::std::u16string_view>& replacements,
        const ::std::vector<::std::u16string_view>& fugenelements_negative
)
    : fugenelements_positive(fugenelements_positive)
    , fugenelements_replaceable(fugenelements_replaceable)
    , replacements(replacements)
    , fugenelements_negative(fugenelements_negative)
{
    ::std::u16string copy;

    for (const auto& fuge : fugenelements_positive) {
        copy = fuge;
        std::reverse(copy.begin(), copy.end());
        this->fugenelements.insert(copy);
        if (int32_t(fuge.length()) > maxFugenelementLength) {
            maxFugenelementLength = int32_t(fuge.length());
        }
    }
    for (const auto& fuge : fugenelements_replaceable) {
        copy = fuge;
        std::reverse(copy.begin(), copy.end());
        this->fugenelements.insert(copy);
        if (int32_t(fuge.length()) > maxFugenelementLength) {
            maxFugenelementLength = int32_t(fuge.length());
        }
    }
}

bool DictionaryTokenizerConfig::isFugenelement(std::u16string_view key) const
{
    return fugenelements.contains(key);
}

bool DictionaryTokenizerConfig::isPositiveFugenelement(std::u16string_view key) const
{
    return fugenelements_positive.contains(key);
}

bool DictionaryTokenizerConfig::isReplaceableFugenelement(std::u16string_view key) const
{
    return fugenelements_replaceable.contains(key);
}

const ::std::vector<::std::u16string_view>& DictionaryTokenizerConfig::getNegativeFugenelements() const
{
    return fugenelements_negative;
}

const ::std::vector<::std::u16string_view>& DictionaryTokenizerConfig::getReplacements() const
{
    return replacements;
}

int32_t DictionaryTokenizerConfig::getMaxCompoundLength() const
{
    return maxCompoundLength;
}

int32_t DictionaryTokenizerConfig::getMinCandidateLength() const
{
    return minCandidateLength;
}

int32_t DictionaryTokenizerConfig::getMinSegmentLength() const
{
    return minSegmentLength;
}

int32_t DictionaryTokenizerConfig::getExpectedSegmentLength() const
{
    return expectedSegmentLength;
}

int32_t DictionaryTokenizerConfig::getMinFrequency() const
{
    return minFrequency;
}

int32_t DictionaryTokenizerConfig::getMinFrequenciesDiff() const
{
    return minFrequenciesDiff;
}

int32_t DictionaryTokenizerConfig::getMaxReplacementFreq() const
{
    return maxReplacementFreq;
}

int32_t DictionaryTokenizerConfig::getMinScore() const
{
    return minScore;
}

float DictionaryTokenizerConfig::getFallbackFreq() const
{
    return fallbackFreq;
}

float DictionaryTokenizerConfig::getUpperMinScoreRatio() const
{
    return upperMinScoreRatio;
}

int32_t DictionaryTokenizerConfig::getMaxFugenelementLength() const
{
    return maxFugenelementLength;
}

int32_t DictionaryTokenizerConfig::getMaxCompoundFreq() const
{
    return maxCompoundFreq;
}

float DictionaryTokenizerConfig::getLowerMinScoreRatio() const
{
    return lowerMinScoreRatio;
}

int32_t DictionaryTokenizerConfig::getMinCompoundLengthForCredibility() const
{
    return minCompoundLengthForCredibility;
}

int32_t DictionaryTokenizerConfig::getMaxDepth() const
{
    return maxDepth;
}

void DictionaryTokenizerConfig::setMinCandidateLength(int32_t minCandidateLength)
{
    this->minCandidateLength = minCandidateLength;
}

void DictionaryTokenizerConfig::setMinSegmentLength(int32_t minSegmentLength)
{
    this->minSegmentLength = minSegmentLength;
}

void DictionaryTokenizerConfig::setMaxCompoundFreq(int32_t maxCompoundFreq)
{
    this->maxCompoundFreq = maxCompoundFreq;
}

void DictionaryTokenizerConfig::setUpperMinScoreRatio(float upperMinScoreRatio)
{
    this->upperMinScoreRatio = upperMinScoreRatio;
}

void DictionaryTokenizerConfig::setLowerMinScoreRatio(float lowerMinScoreRatio)
{
    this->lowerMinScoreRatio = lowerMinScoreRatio;
}

void DictionaryTokenizerConfig::setMinCompoundLengthForCredibility(int32_t minCredibilityLength)
{
    this->minCompoundLengthForCredibility = minCredibilityLength;
}


} // namespace inflection::tokenizer::dictionary
