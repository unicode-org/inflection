/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/TokenIterator.hpp>

#include <morphuntion/tokenizer/Token.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::tokenizer {

TokenIterator::TokenIterator(const Token* head, const Token* end)
 : token(head)
 , end(end)
{
}

const Token*
TokenIterator::operator->() const {
    return token;
}

const Token &
TokenIterator::operator*() const {
    return *token;
}

const Token*
TokenIterator::get() const {
    return token;
}

TokenIterator&
TokenIterator::operator++() {
    token = npc(token)->getNext();
    return *this;
}

TokenIterator
TokenIterator::operator++(int) {
    TokenIterator tmp(token, end);
    ++*this;
    return tmp;
}

TokenIterator&
TokenIterator::operator--() {
    if (token == nullptr) {
        token = end;
    }
    else {
        token = npc(npc(token)->getPrevious());
    }
    return *this;
}

TokenIterator
TokenIterator::operator--(int) {
    TokenIterator tmp(token, end);
    --*this;
    return tmp;
}

bool
TokenIterator::operator==(const TokenIterator& rhs) const {
    return token == rhs.token;
}

bool
TokenIterator::operator!=(const TokenIterator& rhs) const {
    return !(*this == rhs);
}

} // namespace morphuntion
