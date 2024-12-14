/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/Tokenizer.hpp>

#include <morphuntion/tokenizer/iterator/ICUTokenExtractorIterator.hpp>
#include <morphuntion/tokenizer/iterator/NumberCleaverIterator.hpp>
#include <morphuntion/tokenizer/iterator/RegexSplitIterator.hpp>
#include <morphuntion/tokenizer/TokenExtractor.hpp>
#include <morphuntion/tokenizer/TokenUtil.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/LoggerConfig.hpp>
#include <morphuntion/tokenizer/TokenChain.hpp>
#include <morphuntion/tokenizer/Token_Head.hpp>
#include <morphuntion/tokenizer/Token_Tail.hpp>
#include <morphuntion/tokenizer/Token_Word.hpp>
#include <morphuntion/npc.hpp>
#include <algorithm>
#include <memory>

namespace morphuntion::tokenizer {

Tokenizer::~Tokenizer() {
}

Tokenizer::Tokenizer(const tokenizer::TokenExtractor* wordExtractor)
    : super()
    , tokenExtractor(wordExtractor)
{
    Tokenizer::setStyle(TokenizationType::DEFAULT);
}

static constexpr char16_t DASH[] = { u"-" };

TokenChain* Tokenizer::createTokenChain(std::u16string_view charSequence) const
{
    auto& extractor = *npc(this->tokenExtractor);
    int32_t end = int32_t(charSequence.length());
    auto headToken = new Token_Head();
    auto tailToken = new Token_Tail(int32_t(charSequence.length()));
    if (end > 0) {
        std::unique_ptr<tokenizer::iterator::TokenExtractorIterator> tokenExtractorIterator(npc(extractor.createIterator(charSequence)));
        int32_t startIdx = 0;
        int32_t endIdx;
        ::std::u16string lowercase;
        ::std::u16string_view value;
        Token* tail = headToken;
        tokenizer::iterator::TokenExtractorIterator* iteratorPtr = tokenExtractorIterator.get();
        tokenizer::iterator::RegexSplitIterator regexSplitIterator(extractor, charSequence, *iteratorPtr);
        if (extractor.isContainRegex()) {
            iteratorPtr = &regexSplitIterator;
        }
        tokenizer::iterator::NumberCleaverIterator numberCleaverIterator(charSequence, *iteratorPtr);
        if (tokenizationType != TokenizationType::UNMODIFIED) {
            iteratorPtr = &numberCleaverIterator;
        }
        tokenizer::iterator::TokenExtractorIterator& iterator = *iteratorPtr;

        for (; (endIdx = iterator.getNextBoundary()) != UBRK_DONE; startIdx = endIdx) {
            if (startIdx >= endIdx) {
                if (::morphuntion::util::LoggerConfig::isErrorEnabled()) {
                    ::morphuntion::util::Logger::error(u"Boundaries did not progress for locale " + extractor.getLocale().toString() + u" with phrase " + std::u16string(charSequence));
                }
            }
            else {
                value = charSequence.substr(startIdx, endIdx - startIdx);
                if (morphuntion::tokenizer::TokenUtil::containsAllDelimitors(value) || (value == DASH && morphuntion::tokenizer::TokenUtil::isDelimiterPriorToDash(tail))) {
                    tail = morphuntion::tokenizer::TokenUtil::appendToken(tail, new ::morphuntion::tokenizer::Token_Delim(startIdx, endIdx, value, value));
                }
                else {
                    tail = morphuntion::tokenizer::TokenUtil::appendToken(tail,
                            new ::morphuntion::tokenizer::Token_Word(startIdx, endIdx, value,
                                                  *extractor.normalize(&lowercase, value, charSequence, startIdx),
                                                  morphuntion::tokenizer::TokenUtil::isSignificant(value)));
                }
            }
        }
        morphuntion::tokenizer::TokenUtil::appendToken(tail, tailToken);
    }
    else {
        morphuntion::tokenizer::TokenUtil::appendToken(headToken, tailToken);
    }
    auto tokenChain = new TokenChain(headToken, tailToken);
    return tokenChain;
}

void Tokenizer::setStyle(TokenizationType type)
{
    this->tokenizationType = type;
}

} // namespace morphuntion