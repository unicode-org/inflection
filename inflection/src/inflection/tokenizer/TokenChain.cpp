/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/TokenChain.hpp>

#include <inflection/tokenizer/CharRange.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/tokenizer/Token.hpp>
#include <inflection/tokenizer/TokenUtil.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/npc.hpp>

template<typename T, typename U>
static T java_cast(U* u)
{
    if (!u) return static_cast<T>(nullptr);
    auto t = dynamic_cast<T>(u);
    if (!t) throw ::inflection::exception::ClassCastException();
    return t;
}

namespace inflection::tokenizer {

TokenChain::TokenChain(Token_Head* head, Token_Tail* tail)
    : super(static_cast< Token* >(head), static_cast< Token* >(tail), true)
    , tokenArray(npc(tail)->getEndChar() - npc(head)->getStartChar() + 1, nullptr)
{
    headStartChar = npc(head)->getStartChar();
    tokenArray[0] = head;
    tokenArray[npc(head)->getEndChar() - headStartChar] = head;
    tokenArray[npc(tail)->getStartChar() - headStartChar] = tail;
    tokenArray[npc(tail)->getEndChar() - headStartChar] = tail;
    int32_t lastEndChar;
    auto currToken = npc(head)->getNext();
    while (currToken != static_cast< Token* >(tail)) {
        if (npc(currToken)->getLength() > 0) {
            tokenArray[npc(currToken)->getStartChar() - headStartChar] = currToken;
            tokenArray[npc(currToken)->getEndChar() - 1 - headStartChar] = currToken;
        }
        lastEndChar = npc(currToken)->getEndChar();
        currToken = npc(currToken)->getNext();
        if (npc(currToken)->getStartChar() < lastEndChar) {
            break;
        }
    }
}

TokenChain::~TokenChain() {
}

Token_Head* TokenChain::getHead() const
{
    return java_cast< Token_Head* >(start_);
}

Token_Tail* TokenChain::getTail() const
{
    return java_cast< Token_Tail* >(end_);
}

bool TokenChain::operator==(const TokenChain& rhs) const
{
    if (this == &rhs) {
        return true;
    }
    if (getSize() != rhs.getSize()) {
        return false;
    }
    auto leftPointer = getStart();
    auto rightPointer = rhs.getStart();
    while (leftPointer != nullptr) {
        if (*leftPointer != *rightPointer) {
            return false;
        }
        leftPointer = npc(leftPointer)->getNext();
        rightPointer = npc(rightPointer)->getNext();
    }
    return rightPointer == nullptr;
}

std::size_t TokenChain::operator()(const TokenChain& tokenChain) const noexcept
{
    std::size_t hash = 0;
    for (const Token* head = tokenChain.start_; head != nullptr; head = head->getNext()) {
        hash = (hash * 31) + (*head)(*head);
    }
    return hash;
}

TokenChain* TokenChain::getNewTokenChain(int32_t start, int32_t end) const
{
    auto head = java_cast< Token_Head* >(npc(getHead())->newInstance(start));
    Token* prev = head;
    const Token* curr = getHead();
    while (npc(curr)->getNext() != static_cast<const Token* >(getTail())) {
        curr = npc(curr)->getNext();
        if (npc(curr)->getStartChar() < start || npc(curr)->getEndChar() > end) {
            continue;
        }
        auto token = npc(curr)->newInstance(start);
        npc(token)->setPrevious(prev);
        npc(prev)->setNext(token);
        prev = token;
    }
    auto tail = new Token_Tail(npc(prev)->getEndChar());
    npc(tail)->setPrevious(prev);
    npc(prev)->setNext(tail);
    return new TokenChain(head, tail);
}

const ::std::u16string& TokenChain::getTokenValue(int32_t index) const
{
    auto token = getToken(index);
    if (token == nullptr) {
        return getHead()->getValue();
    }
    return npc(token)->getValue();
}

Token* TokenChain::getToken(int32_t index) const
{
    if (index == -1) {
        return static_cast< Token* >(this->getHead());
    }
    if (0 <= (index - headStartChar) && (index - headStartChar) < int32_t(this->tokenArray.size())) {
        return this->tokenArray[index - headStartChar];
    }
    if (util::LoggerConfig::isErrorEnabled()) {
        util::Logger::error(u"Attempting to getToken with invalid index: " + util::StringUtils::to_u16string(index));
    }
    return nullptr;
}

std::u16string TokenChain::toString() const
{
    std::u16string retVal(u"Tokens: ");
    auto curr = this->start_;
    auto added = false;
    while (curr != nullptr && curr != this->end_) {
        if (!npc(curr)->isWhitespace()) {
            if (added) {
                retVal.append(u" | ");
            }
            retVal.append(npc(curr)->getValue());
            added = true;
        }
        curr = npc(curr)->getNext();
    }
    return retVal;
}

std::u16string TokenChain::toOriginatingString() const
{
    std::u16string retVal;
    auto curr = this->start_;
    int32_t lastIndex = -1;
    while (curr != nullptr && curr != this->end_) {
        if(npc(curr)->getStartChar() >= lastIndex){
            retVal.append(npc(curr)->getValue());
            lastIndex = npc(curr)->getEndChar();
        }
        curr = npc(curr)->getNext();
    }
    return retVal;
}

} // namespace inflection
