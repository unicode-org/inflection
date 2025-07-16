/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/Chunk.hpp>

#include <inflection/tokenizer/Token_Delim.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/Token.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/NullPointerException.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::tokenizer {

Chunk::Chunk(Token* start, Token* end, bool adoptTokens)
    : super()
    , start_(start)
    , end_(end)
    , ownTokens(adoptTokens)
{
    this->size = checkSize(start, end);
}

Chunk::~Chunk()
{
    if (ownTokens) {
        std::unique_ptr<Token> curr(this->start_);
        while (curr != nullptr) {
            curr.reset(curr->getNext());
        }
    }
}

int32_t Chunk::getWordCount(int32_t start, int32_t end) const
{
    ::std::unique_ptr<Chunk> chunk(getSubChunk(start, end));
    return chunk->getWordCount();
}

int32_t Chunk::getWordCount() const
{
    return getWordCount(this);
}

int32_t Chunk::getWordCount(const Chunk* chunk)
{
    int32_t numWords = 0;
    for (auto token = npc(chunk)->start_, chunkEnd = npc(chunk)->end_; token != nullptr && token != chunkEnd; token = npc(token->getNext())) {
        if (npc(token)->isSignificant()) {
            numWords++;
        }
    }
    return numWords;
}

int32_t Chunk::checkSize(const Token* start, const Token* end)
{
    if (start == nullptr || end == nullptr) {
        throw exception::NullPointerException();
    }
    int32_t c = 1;
    const Token* t = start;
    while (t != end) {
        c++;
        t = npc(t)->getNext();
        if (t == nullptr) {
            throw exception::IllegalArgumentException(u"end token must be linked to and eventually follow start token!");
        }
    }
    return c;
}

int32_t Chunk::getSize() const
{
    return size;
}

Token* Chunk::getStart() const
{
    return start_;
}

Token* Chunk::getEnd() const
{
    return end_;
}

Chunk* Chunk::getSubChunk(int32_t start, int32_t end) const
{
    Token* startToken = nullptr;
    Token* endToken = nullptr;
    const auto* beyondChunkEndToken = npc(end_)->getNext();
    for (auto token = start_; token != nullptr && token != beyondChunkEndToken ; token = token->getNext()) {
        if (startToken == nullptr && start >= npc(token)->getStartChar() && start < npc(token)->getEndChar()) {
            startToken = token;
        }
        if (endToken == nullptr && end > npc(token)->getStartChar() && end <= npc(token)->getEndChar()) {
            endToken = token;
        }
        if (startToken != nullptr && endToken != nullptr) {
            break;
        }
    }
    if (endToken == nullptr || startToken == nullptr) {
        return nullptr;
    }
    return new Chunk(startToken, endToken);
}

::std::u16string Chunk::getCleanValueString()
{
    return getCleanValueString(true);
}

::std::u16string Chunk::getCleanValueString(bool normalizeDelim)
{
    ::std::u16string result;
    const auto* end = npc(end_)->getNext();
    for (const Token* token = start_; token != nullptr && token != end; token = token->getNext()) {
        if (normalizeDelim) {
            if (!npc(token)->isSignificant() && npc(token)->getCleanValue() == ::std::u16string(u"-")) {
                result.append(u" ");
            } else {
                result.append(npc(token)->getCleanValue());
            }
        } else {
            result.append(npc(token)->getCleanValue());
        }
    }
    return result;
}

::std::u16string Chunk::getCleanString()
{
    return getNormalizedString(true);
}

::std::u16string Chunk::getString() const
{
    return getNormalizedString(false);
}

::std::u16string Chunk::getNormalizedString(bool normalizeDelim) const
{
    ::std::u16string result;
    const auto* end = npc(end_)->getNext();
    for (const auto* token = start_; token != nullptr && token != end; token = token->getNext()) {
        if (dynamic_cast<const Token_Word* >(token) != nullptr) {
            result.append(npc(token)->getValue());
        } else if (dynamic_cast<const Token_Delim* >(token) != nullptr) {
            if (normalizeDelim) {
                if (npc(token)->getLength() > 0) {
                    result.append(u" ");
                }
            } else {
                result.append(npc(token)->getValue());
            }
        }
    }
    if (normalizeDelim) {
        return std::u16string(util::StringViewUtils::trim(result));
    }
    return result;
}

::std::u16string Chunk::toString() const
{
    return getString();
}

} // namespace inflection
