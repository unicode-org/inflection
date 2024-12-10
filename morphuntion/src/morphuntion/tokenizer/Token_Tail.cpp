/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/Token_Tail.hpp>

namespace morphuntion::tokenizer {

Token_Tail::Token_Tail(int32_t utteranceLength)
    : super(utteranceLength, utteranceLength, std::u16string_view(u""), std::u16string_view(u""))
{
}

Token* Token_Tail::newInstance(int32_t offset) const
{
    return new Token_Tail(this->getStartChar() - offset);
}

} // namespace morphuntion
