/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/GermanicDecompounder.hpp>

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/dictionary/Segment.hpp>
#include <inflection/npc.hpp>
#include <unicode/uchar.h>
#include <memory>

namespace inflection::tokenizer::dictionary {

GermanicDecompounder::GermanicDecompounder(const DictionaryTokenizerConfig& config, const ::std::u16string& dictionaryPath)
    : config(config)
    , corpus(dictionaryPath)
    , segmentValidator(this->config, corpus)
    , parsingsScorer(this->config)
    , maxDepth(this->config.getMaxDepth())
    , maxCompoundLengthMinusOne(this->config.getMaxCompoundLength() - 1)
    , minCandidateLength(this->config.getMinCandidateLength())
    , maxCompoundFreq(this->config.getMaxCompoundFreq())
{
}

GermanicDecompounder::~GermanicDecompounder() = default;

void GermanicDecompounder::analyze(const Segment::SharedPtr& parent,
                                   const Segment::SharedPtr& word,
                                   int32_t offset,
                                   int32_t wordEnd,
                                   bool isStringTail,
                                   ::std::vector<Segment::SharedPtr>* leafs,
                                   int32_t trailingFugeLen) const
{
    word->tie(parent);
    npc(leafs)->push_back(word);
    if (word->isNoCompound() || word->isEnglish() || word->getFreq() > maxCompoundFreq || word->getDepth() > maxDepth) {
        return;
    }
    auto compound = word->getCompound();
    Segment::SharedPtr prev = nullptr;
    Segment::SharedPtr next = nullptr;
    auto wordLength = wordEnd - offset;
    auto upperBoundary = wordLength - minCandidateLength;
    for (auto k = minCandidateLength; k <= upperBoundary && k < maxCompoundLengthMinusOne; k++) {
        auto splitPoint = wordEnd - k;
        auto first = next != nullptr ? next : _newSegment(compound, splitPoint, wordEnd, trailingFugeLen);
        next = k < upperBoundary ? _newSegment(compound, splitPoint - 1, wordEnd, trailingFugeLen) : nullptr;
        if (segmentValidator.validateFirst(first.get(), prev.get(), next.get(), isStringTail)) {
            auto second(_newSegment(compound, offset, splitPoint));
            if (segmentValidator.validateSecond(second.get()) && !(isStringTail && first->isNoHead())) {
                auto secondFugeLen = second->getFugeLen();
                auto effectiveEnd = splitPoint - (secondFugeLen > 0 ? secondFugeLen : 0);
                first->tie(parent);
                analyze(first, second, offset, effectiveEnd, false, leafs, secondFugeLen > 0 ? secondFugeLen : 0);
            }
        }
        prev = first;
    }
}

Segment::SharedPtr GermanicDecompounder::_newSegment(::std::u16string_view compound, int32_t start, int32_t end) const
{
    return ::std::make_shared<Segment>(config, corpus, compound, start, end);
}

Segment::SharedPtr GermanicDecompounder::_newSegment(::std::u16string_view compound, int32_t start, int32_t end, int32_t trailingFugeLen) const
{
    return ::std::make_shared<Segment>(config, corpus, compound, start, end + trailingFugeLen);
}

void GermanicDecompounder::decompound(std::vector<int32_t>* boundaries, std::u16string_view phrase, int32_t start, int32_t length) const
{
    ::std::u16string compound;
    compound.reserve(length);
    for (const auto ch : phrase.substr(start, length)) {
        /*
         * Naively lower cases a string so that indexes don't change.
         * This does not work correctly for supplementary characters and uppercasing the sharp S
         */
        compound.push_back(static_cast<char16_t>(u_tolower(ch)));
    }
    ::std::vector<Segment::SharedPtr> segments;
    auto root = _newSegment(compound, 0, int32_t(compound.length()));
    Segment::SharedPtr wordSegment(::std::make_shared<Segment>(*npc(root.get())));
    analyze(root, wordSegment, 0, int32_t(compound.length()), true, &segments);

    parsingsScorer.getBestOffsets(boundaries, start, &segments);
}

int32_t GermanicDecompounder::getMinimumCandidateLength() const
{
    return minCandidateLength;
}

} // namespace inflection::tokenizer::dictionary
