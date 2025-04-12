/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/SegmentFuger.hpp>

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::tokenizer::dictionary {

SegmentFuger::SegmentFuger(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus)
    : config(config)
    , corpus(corpus)
    , maxFugenelementLength(config.getMaxFugenelementLength())
    , minSegmentLength(config.getMinSegmentLength())
    , maxReplacementFreq(config.getMaxReplacementFreq())
{
}

void SegmentFuger::detachFugenelement(Segment* word) const
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
        root = term.substr(fugeLength);
        fuge = term.substr(0, fugeLength);
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
                    possibleRoot = negFuge;
                    possibleRoot.append(std::u16string(root));
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
                    possibleRoot = repFuge;
                    possibleRoot.append(std::u16string(root));
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
                possibleRoot = negFuge;
                possibleRoot += term;
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
