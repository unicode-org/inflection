/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/ParsingsScorer.hpp>

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/dictionary/GermanicDecompounder.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <inflection/util/StringUtils.hpp>
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
    auto compoundLength = segment->getCompoundLength();
    auto compoundStart = offset;
    int32_t subIndex;
    while ((parent = segment->getParent()) != nullptr) {
        auto segmentOffset = segment->getOffset();
        if (segmentOffset != 0) {
            auto rootOffset = segment->getRootOffset();
            if (segmentOffset != rootOffset) {
                subIndex = compoundLength - segment->getRootOffset();
                npc(boundaries)->emplace_back(compoundStart + subIndex);
            }
            subIndex = compoundLength - segmentOffset;
            npc(boundaries)->emplace_back(compoundStart + subIndex);
        }
        segment = parent;
    }
}

Segment::SharedPtr ParsingsScorer::_getBestLeaf(::std::vector<Segment::SharedPtr>* leafs) const
{
    for (const auto& leaf : *npc(leafs)) {
        leaf->score();
    }
    auto defaultParsing = (*npc(leafs))[0];
    ::std::sort(npc(leafs)->begin(), npc(leafs)->end(),
                [](const Segment::SharedPtr& first, const Segment::SharedPtr& second)
                {
                    return second->getScore() < first->getScore();
                });
    auto bestLeaf = (*npc(leafs))[0];
    if (bestLeaf->getScore() < minScore && bestLeaf->getArithmeticMeanLength() < expectedSegmentLength) {
        bestLeaf = defaultParsing;
    } else {
        auto bestLeafId = _prune(*npc(leafs));
        bestLeaf = (*npc(leafs))[bestLeafId];
    }
    return bestLeaf;
}

int32_t ParsingsScorer::_prune(const ::std::vector<Segment::SharedPtr>& leafs) const
{
    auto stop = false;
    int32_t bestLeafId = 0;

    while (!stop) {
        stop = true;
        auto best = leafs[bestLeafId];
        int32_t numLeafs = int32_t(leafs.size());
        auto bestScore = best->getScore();
        auto bestDepth = static_cast< float >(best->getDepth());
        auto bestGeometricMeanLength = best->getGeometricMeanLength();
        auto bestCredibility = best->getCredibility();
        for (int32_t i = bestLeafId + 1; i < numLeafs; i++) {
            auto next = leafs[i];
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
