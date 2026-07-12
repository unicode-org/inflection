/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/ParsingsScorer.hpp>

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/dictionary/GermanicDecompounder.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <inflection/npc.hpp>
#include <cmath>
#include <algorithm>

namespace inflection::tokenizer::dictionary {

static constexpr float LOWER_MIN_CREDIBILITY { 0 };
static constexpr float UPPER_MIN_CREDIBILITY { 3 };
static constexpr float MIN_CREDIBILITY_DIFF { 3 };

ParsingsScorer::ParsingsScorer(const DictionaryTokenizerConfig& config)
    : minScore(config.getMinScore())
    , expectedSegmentLength(config.getExpectedSegmentLength())
    , lowerMinScoreRatio(config.getLowerMinScoreRatio())
    , upperMinScoreRatio(config.getUpperMinScoreRatio())
    , minCompoundLengthForCredibility(config.getMinCompoundLengthForCredibility())
{
}

void ParsingsScorer::getBestOffsets(std::vector<int32_t>* boundaries, int32_t offset, ::std::vector<Segment::SharedPtr>* leafs) const
{
    auto segment = _getBestLeaf(leafs);
    Segment::SharedPtr parent;
    auto compoundStart = offset;
    while ((parent = segment->getParent()) != nullptr) {
        auto segmentOffset = segment->getOffset();
        if (segmentOffset != 0) {
            npc(boundaries)->emplace_back(compoundStart + segmentOffset);
        }
        auto fugeLen = segment->getFugeLen();
        if (fugeLen > 0) {
            npc(boundaries)->emplace_back(compoundStart + parent->getOffset() - fugeLen);
        }
        segment = parent;
    }
}

Segment::SharedPtr ParsingsScorer::_getBestLeaf(::std::vector<Segment::SharedPtr>* leafs) const
{
    float bestScore = -1.0f;
    Segment::SharedPtr bestLeaf;
    for (const auto& leaf : *npc(leafs)) {
        auto score = leaf->score();
        if (score > bestScore) {
            bestScore = score;
            bestLeaf = leaf;
        }
    }
    if (npc(leafs)->size() == 1
        || (bestScore < minScore && bestLeaf->getArithmeticMeanLength() < expectedSegmentLength))
    {
        return npc(leafs)->front();
    }
    std::ranges::sort(*npc(leafs),
                      [](const Segment::SharedPtr& first, const Segment::SharedPtr& second)
                      {
                          return second->getScore() < first->getScore();
                      });
    auto bestLeafId = _prune(*npc(leafs));
    return (*npc(leafs))[bestLeafId];
}

int32_t ParsingsScorer::_prune(const ::std::vector<Segment::SharedPtr>& leafs) const
{
    auto stop = false;
    int32_t bestLeafId = 0;

    while (!stop) {
        stop = true;
        const auto& best = leafs.at(bestLeafId);
        int32_t numLeafs = int32_t(leafs.size());
        auto bestScore = best->getScore();
        auto bestDepth = static_cast< float >(best->getDepth());
        auto bestGeometricMeanLength = best->getGeometricMeanLength();
        auto bestCredibility = best->getCredibility();
        for (int32_t i = bestLeafId + 1; i < numLeafs; i++) {
            const auto& next = leafs.at(i);
            auto diffDepth = bestDepth - next->getDepth();
            auto nextCredibility = next->getCredibility();
            if ((bestDepth > next->getDepth() && bestScore * ::std::pow(upperMinScoreRatio, diffDepth) < next->getScore()) || (next->getDepth() > 1 && bestGeometricMeanLength < next->getGeometricMeanLength() && bestScore * upperMinScoreRatio < next->getScore()) || (best->getCompoundLength() >= minCompoundLengthForCredibility && ((bestCredibility < LOWER_MIN_CREDIBILITY && nextCredibility >= LOWER_MIN_CREDIBILITY) || (bestCredibility < UPPER_MIN_CREDIBILITY && nextCredibility >= UPPER_MIN_CREDIBILITY && bestScore * ::std::pow(lowerMinScoreRatio, diffDepth) < next->getScore()) || (bestScore < minScore && bestCredibility < LOWER_MIN_CREDIBILITY && nextCredibility - bestCredibility >= MIN_CREDIBILITY_DIFF)))) {
                bestLeafId++;
                stop = false;
                break;
            }
        }
    }
    return bestLeafId;
}

} // namespace inflection::tokenizer::dictionary
