/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/SegmentValidator.hpp>

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <inflection/npc.hpp>

namespace inflection::tokenizer::dictionary {

SegmentValidator::SegmentValidator(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus)
    : config(config)
    , corpus(corpus)
    , maxFugenelementLength(config.getMaxFugenelementLength())
    , maxReplacementFreq(config.getMaxReplacementFreq())
    , minFrequenciesDiff(config.getMinFrequenciesDiff())
    , minSegmentLength(config.getMinSegmentLength())
    , minFrequency(config.getMinFrequency())
{
}

bool SegmentValidator::validateFirst(Segment* segment, Segment* prev, Segment* next, bool isStringTail) const
{
    if (isStringTail) {
        if (npc(segment)->isNoHead()) {
            return false;
        }
    } else if (npc(segment)->getFugeLen() < 0) {
        detachFugenelement(segment);
    }
    auto freq = npc(segment)->getFreq();
    return (npc(segment)->getLength() >= minSegmentLength || npc(segment)->isSegment()) && !npc(segment)->isNoAtomic() && (freq >= minFrequency || (prev != nullptr && next != nullptr && freq >= npc(prev)->getFreq() + minFrequenciesDiff && freq >= npc(next)->getFreq() + minFrequenciesDiff));
}

bool SegmentValidator::validateSecond(Segment* segment) const
{
    detachFugenelement(segment);
    return (npc(segment)->getLength() >= minSegmentLength || npc(segment)->isSegment()) && !npc(segment)->isNoAtomic();
}

void SegmentValidator::detachFugenelement(Segment* word) const
{
    auto term(npc(word)->getString());
    int32_t termLength = int32_t(term.length());
    if (termLength < minSegmentLength) {
        return;
    }
    auto freq = npc(word)->getFreq();
    int32_t data = 0;
    auto found = false;
    std::u16string_view root;
    std::u16string_view fuge;
    std::u16string possibleRoot;
    possibleRoot.reserve(maxFugenelementLength);
    for (auto fugeLength = maxFugenelementLength; fugeLength > 0 && termLength >= fugeLength + minSegmentLength - 1; fugeLength--) {
        root = term.substr(0, termLength - fugeLength);
        fuge = term.substr(termLength - fugeLength);
        bool hasFuge = !fuge.empty();
        if (config.isFugenelement(fuge)) {
            if (config.isPositiveFugenelement(fuge)) {
                auto _data = corpus.trie.find(root).value_or(0);
                int32_t _freq = _data & 0x00FFFFFF;
                auto success = false;
                if (_freq > freq && (((int32_t)root.length() >= minSegmentLength) || (((_data >> 25) & 1) == 1))) {
                    success = true;
                    found = true;
                }
                for (const auto& negFuge : config.getNegativeFugenelements()) {
                    possibleRoot.assign(root);
                    possibleRoot.append(negFuge);
                    auto j__data = corpus.trie.find(possibleRoot).value_or(0);
                    int32_t j__freq = j__data & 0x00FFFFFF;
                    if (j__freq > freq && j__freq > _freq && ((int32_t)possibleRoot.length() >= minSegmentLength || (((j__data >> 25) & 1) == 1))) {
                        root = possibleRoot;
                        _freq = j__freq;
                        _data = j__data;
                        success = true;
                        found = true;
                    }
                }
                if (success) {
                    data = _data;
                    freq = _freq;
                }
            }
            if (config.isReplaceableFugenelement(fuge) && freq <= maxReplacementFreq) {
                auto _freq = freq;
                for (const auto& repFuge : config.getReplacements()) {
                    possibleRoot.assign(root);
                    possibleRoot.append(repFuge);
                    auto j__data = corpus.trie.find(possibleRoot).value_or(0);
                    int32_t j__freq = j__data & 0x00FFFFFF;
                    if (j__freq > _freq) {
                        root = possibleRoot;
                        _freq = j__freq;
                        data = j__data;
                        found = true;
                    }
                }
            }
        }
        else if (npc(word)->getLength() >= minSegmentLength) {
            auto _freq = freq;
            for (const auto& negFuge : config.getNegativeFugenelements()) {
                possibleRoot.assign(term);
                possibleRoot.append(negFuge);
                auto j__data = corpus.trie.find(possibleRoot).value_or(0);
                int32_t j__freq = j__data & 0x00FFFFFF;
                if (j__freq > _freq && ((int32_t)possibleRoot.length() >= minSegmentLength || (((j__data >> 25) & 1) == 1))) {
                    root = possibleRoot;
                    _freq = j__freq;
                    data = j__data;
                    found = true;
                    hasFuge = false;
                }
            }
        }
        if (found) {
            npc(word)->setRootAndFuge(root, int32_t(hasFuge ? fuge.length() : -1), data);
            break;
        }
    }
}

} // namespace inflection::tokenizer::dictionary
