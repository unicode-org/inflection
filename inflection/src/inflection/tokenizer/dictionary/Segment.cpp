/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/dictionary/Segment.hpp>

#include <inflection/tokenizer/dictionary/DictionaryTokenizerConfig.hpp>
#include <inflection/tokenizer/trie/SerializedTrie.hpp>
#include <cmath>

namespace inflection::tokenizer::dictionary {


Segment::Segment(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus, ::std::u16string_view compound, int32_t start, int32_t end)
    : Segment(config, corpus, compound, start, end, start, -1)
{
}

Segment::Segment(const DictionaryTokenizerConfig& config, const ::inflection::tokenizer::trie::SerializedTrie& corpus, ::std::u16string_view compound, int32_t start, int32_t end, int32_t rootStart, int32_t fugeLen)
    : compound(compound)
    , start(start)
    , end(end)
    , rootStart(rootStart)
    , fugeLen(fugeLen)
    , expectedSegmentLength(config.getExpectedSegmentLength())
{
    auto data(corpus.trie.find(compound.substr(rootStart, end - rootStart)));
    if (data) {
        _parseData(*data);
    } else {
        freq = config.getFallbackFreq();
    }
}

Segment::Segment(const Segment& other)
    : compound(other.compound)
    , start(other.start)
    , end(other.end)
    , rootStart(other.rootStart)
    , fugeLen(other.fugeLen)
    , root(other.root)
    , freq(other.freq)
    , isSegment_(other.isSegment_)
    , isNoAtomic_(other.isNoAtomic_)
    , isNoHead_(other.isNoHead_)
    , isNoCompound_(other.isNoCompound_)
    , isEnglish_(other.isEnglish_)
    , parent(other.parent)
    , freqProduct(other.freqProduct)
    , depth(other.depth)
    , splits(other.splits)
    , lengthProduct(other.lengthProduct)
    , score_(other.score_)
    , credibility(other.credibility)
    , expectedSegmentLength(other.expectedSegmentLength)
{
}

Segment::~Segment()
{
}

void Segment::_parseData(int32_t data)
{
    // AND with 16 1-bits such that only the rightmost 16 bits remain. They encode the frequency (which can
    // consequently never be higher than 65.535).
    freq = (float)(data & 0x00FFFFFF);

    // Bitshift right 16 bits to extract the integer that encodes the booleans.
    auto bools = data >> 24;

    this->isNoAtomic_ = (bools >> 4) & 1;
    this->isNoHead_ = (bools >> 3) & 1;
    this->isNoCompound_ = (bools >> 2) & 1;
    this->isSegment_ = (bools >> 1) & 1;
    this->isEnglish_ = (bools & 1);
}

void Segment::setRootAndFuge(std::u16string_view root, int32_t fugeLen, int32_t data)
{
    this->root = root;
    if (fugeLen > 0) {
        this->rootStart = start + fugeLen;
        this->fugeLen = fugeLen;
    }
    _parseData(data);
}

void Segment::tie(Segment::SharedPtr parent)
{
    auto length = int32_t(root.empty() ? end - rootStart : root.length());
    this->parent = parent;
    freqProduct = parent->getFreqProduct() * freq;
    lengthProduct = parent->getLengthProduct() * length;
    depth = parent->getDepth() + 1;
    splits = parent->getSplits() + (fugeLen < 0 ? 1 : 2);
    credibility = parent->getCredibility() + length - expectedSegmentLength - (root.empty() ? 0.0f : 0.5f);
}

float Segment::getGeometricMeanLength() const
{
    return static_cast< float >(::pow(static_cast< double >(lengthProduct), 1.0 / static_cast< double >(depth)));
}

int32_t Segment::getCompoundLength() const
{
    return int32_t(compound.length());
}

float Segment::getArithmeticMeanLength() const
{
    return (static_cast< float >(compound.length())) / static_cast< float >(depth);
}

void Segment::score()
{
    score_ = static_cast< float >(::pow(static_cast< double >(freqProduct), 1.0 / (static_cast< float >(depth) * 1.5f)));
}

::std::u16string_view Segment::getCompound() const
{
    return compound;
}

int32_t Segment::getOffset() const
{
    return start;
}

int32_t Segment::getRootOffset() const
{
    return rootStart;
}

float Segment::getFreq() const
{
    return freq;
}

bool Segment::isSegment() const
{
    return isSegment_;
}

bool Segment::isNoAtomic() const
{
    return isNoAtomic_;
}

bool Segment::isNoHead() const
{
    return isNoHead_;
}

bool Segment::isNoCompound() const
{
    return isNoCompound_;
}

bool Segment::isEnglish() const
{
    return isEnglish_;
}

int32_t Segment::getLength() const
{
    return root.empty() ? end - rootStart : int32_t(root.length());
}

float Segment::getFreqProduct() const
{
    return freqProduct;
}

int32_t Segment::getDepth() const
{
    return depth;
}

Segment::SharedPtr Segment::getParent() const
{
    return parent;
}

float Segment::getScore() const
{
    return score_;
}

::std::u16string_view Segment::getString() const
{
    return compound.substr(start, end - start);
}

int32_t Segment::getFugeLen() const
{
    return fugeLen;
}

int32_t Segment::getLengthProduct() const
{
    return lengthProduct;
}

float Segment::getCredibility() const
{
    return credibility;
}

int32_t Segment::getSplits() const
{
    return splits;
}


} // namespace inflection::tokenizer::dictionary
