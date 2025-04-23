/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/SegmentValidator.hpp>

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <inflection/npc.hpp>

namespace inflection::tokenizer::dictionary {

SegmentValidator::SegmentValidator(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus)
    : segmentFuger(config, corpus)
    , minFrequenciesDiff(config.getMinFrequenciesDiff())
    , minSegmentLength(config.getMinSegmentLength())
    , minFrequency(config.getMinFrequency())
{
}

SegmentValidator::~SegmentValidator()
{
}

bool SegmentValidator::validateFirst(Segment* segment, Segment* prev, Segment* next, bool isStringTail) const
{
    if (isStringTail) {
        if (npc(segment)->isNoHead()) {
            return false;
        }
    } else if (npc(segment)->getFugeLen() < 0) {
        segmentFuger.detachFugenelement(segment);
    }
    auto freq = npc(segment)->getFreq();
    return (npc(segment)->getLength() >= minSegmentLength || npc(segment)->isSegment()) && !npc(segment)->isNoAtomic() && (freq >= minFrequency || (prev != nullptr && next != nullptr && freq >= npc(prev)->getFreq() + minFrequenciesDiff && freq >= npc(next)->getFreq() + minFrequenciesDiff));
}

bool SegmentValidator::validateSecond(Segment* segment) const
{
    segmentFuger.detachFugenelement(segment);
    return (npc(segment)->getLength() >= minSegmentLength || npc(segment)->isSegment()) && !npc(segment)->isNoAtomic();
}

} // namespace inflection::tokenizer::dictionary
