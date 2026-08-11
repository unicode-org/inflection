/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/fi/FiCompound.hpp>

#include <inflection/tokenizer/locale/fi/FiSegment.hpp>
#include <inflection/tokenizer/locale/fi/FiSuffixSplit.hpp>
#include <inflection/tokenizer/locale/fi/FiTokenizerConfig.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include <inflection/exception/ExcessComplexityException.hpp>
#include <inflection/npc.hpp>

namespace inflection::tokenizer::locale {
namespace fi {

FiCompound::FiCompound(FiSuffixSplit* suffixSplit)
    : segment(npc(suffixSplit)->getSegment())
    , suffixSplit(suffixSplit)
    , logProb(npc(suffixSplit)->getLogProb())
    , found(npc(suffixSplit)->found())
{
}

float FiCompound::getLogProb() const
{
    return logProb;
}

void FiCompound::addBoundaries(int32_t stackDepthLeft, ::std::vector<int32_t>* max_indexes)
{
    if (stackDepthLeft <= 0) {
        throw ::inflection::exception::ExcessComplexityException();
    }
    if (npc(suffixSplit)->found()) {
        this->logProb = npc(suffixSplit)->getLogProb();
        npc(suffixSplit)->addBoundaries(max_indexes, FiSuffixSplit::SuffixType::SUFFIX);
        return;
    }
    this->logProb = 2 * FiTokenizerConfig::MINLOGPROB;
    this->found = false;
    auto const start = npc(suffixSplit)->getStart();
    auto const end = npc(suffixSplit)->getEnd();
    auto const stopIdx = end - FiTokenizerConfig::MINSTEMLENGTH + 1;
    for (auto i = start + FiTokenizerConfig::MINSTEMLENGTH; i < stopIdx; i++) {
        float logProb1;
        float logProb2;
        bool found1;
        auto new_found = false;
        FiSegment segment1(segment, start, i, false);
        ::std::vector<int32_t> indexes1;
        segment1.addBoundaries(&indexes1);
        if (segment1.found()) {
            logProb1 = segment1.getLogProb();
            found1 = true;
        } else {
            FiSuffixSplit suffixSplit1(&segment1);
            indexes1.clear();
            suffixSplit1.addBoundaries(&indexes1, FiSuffixSplit::SuffixType::MIDDLE_SUFFIX);
            found1 = suffixSplit1.found();
            if (found && !found1) {
                continue;
            }
            logProb1 = suffixSplit1.getLogProb();
        }
        FiSegment segment2(segment, i, end, true);
        ::std::vector<int32_t> indexes2;
        segment2.addBoundaries(&indexes2);
        if (segment2.found()) {
            if (found1) {
                new_found = true;
            }
            logProb2 = segment2.getLogProb();
        } else {
            FiSuffixSplit suffixSplit2(&segment2);
            indexes2.clear();
            suffixSplit2.addBoundaries(&indexes2, FiSuffixSplit::SuffixType::SUFFIX);
            logProb2 = suffixSplit2.getLogProb();
            if (suffixSplit2.found()) {
                if (found1) {
                    new_found = true;
                }
            } else {
                if (!found && found1 && FiTokenizerConfig::MIN_FICOMPOUNDSPLIT <= segment2.getLength()) {
                    FiCompound compound2(&suffixSplit2);
                    indexes2.clear();
                    compound2.addBoundaries(stackDepthLeft - 1, &indexes2);
                    logProb2 = compound2.getLogProb();
                }
            }
        }
        if ((logProb1 + logProb2 > this->logProb) || (!this->found && new_found)) {
            npc(max_indexes)->clear();
            if (!indexes1.empty()) {
                npc(max_indexes)->insert(npc(max_indexes)->end(), indexes1.begin(), indexes1.end());
            }
            npc(max_indexes)->push_back(i);
            if (!indexes2.empty()) {
                npc(max_indexes)->insert(npc(max_indexes)->end(), indexes2.begin(), indexes2.end());
            }
            this->logProb = logProb1 + logProb2;
            if (new_found) {
                this->found = true;
            }
        }
    }
}

} // namespace tokenizer::locale::fi
} // namespace inflection
