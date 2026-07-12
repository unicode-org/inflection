/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/fi/FiSegment.hpp>

#include <inflection/tokenizer/locale/fi/FiSuffixSplit.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include "inflection/tokenizer/locale/fi/FreqToLogProb.h"
#include <inflection/npc.hpp>

namespace inflection::tokenizer::locale::fi {

static constexpr int32_t MAX_WORD_FREQ = std::ssize(FREQ_TO_LOG_PROB) - 1;

FiSegment::FiSegment(const FiSegment* parent, int32_t start, int32_t end, bool isTail)
    : FiSegment(npc(parent)->corpus, npc(parent)->originalCompound, start, end, isTail)
{
}

FiSegment::FiSegment(const ::inflection::tokenizer::trie::SerializedTrie& corpus, ::std::u16string_view compound, int32_t start, int32_t end, bool isTail)
    : corpus(corpus)
    , label(compound.substr(start, end - start))
    , originalCompound(compound)
    , start(start)
    , end(end)
    , isTail(isTail)
{
    auto lookupVal = corpus.trie.find(label);
    if (lookupVal) {
        int32_t data = *lookupVal;
        auto freq = data & 0x0000FFFF;
        if (freq > MAX_WORD_FREQ) {
            freq = MAX_WORD_FREQ;
        } else if (freq < 2) {
            freq = 2;
        }
        this->logProb = FREQ_TO_LOG_PROB[freq - 1];
        this->breakPoint = (data & 0x003F0000) >> 16;
        this->isCompound = (data & 0x00400000) != 0;
        this->found_ = true;
    } else {
        this->logProb = 0.0f;
        this->breakPoint = 0;
        this->isCompound = false;
        this->found_ = false;
    }
}

::std::u16string_view FiSegment::getLabel() const
{
    return this->label;
}

bool FiSegment::found() const
{
    return found_;
}

float FiSegment::getLogProb() const
{
    return logProb;
}

int32_t FiSegment::getStart() const
{
    return start;
}

int32_t FiSegment::getEnd() const
{
    return end;
}

int32_t FiSegment::getLength() const
{
    return end - start;
}

void FiSegment::addBoundaries(::std::vector<int32_t>* indexes) const
{
    if (!this->found_ || breakPoint == 0) {
        return;
    }
    auto this_id = end - breakPoint;
    FiSegment headSegment(this, start, this_id, false);
    headSegment.addBoundaries(indexes);
    FiSegment tailSegment(this, this_id, end, true);
    if (this->isCompound && !FiSuffixSplit::isSuffix(tailSegment.getLabel())) {
        npc(indexes)->push_back(this_id);
        tailSegment.addBoundaries(indexes);
    }
    else {
        if (this->isTail && FiSuffixSplit::isInNotNominative(this, tailSegment.getLabel())) {
            FiSuffixSplit::addSuffixInfo(tailSegment.getLabel(), indexes, this_id);
        }
    }
}

} // namespace inflection::tokenizer::locale::fi
