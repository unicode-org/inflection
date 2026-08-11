/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/Token_Head.hpp>

namespace inflection::tokenizer {

Token_Head::Token_Head()
    : Token_Head(0)
{
}

Token_Head::Token_Head(int32_t start) 
    : super(start, start, std::u16string_view(u""), std::u16string_view(u""))
{
}

Token* Token_Head::newInstance(int32_t offset) const
{
    if(getStartChar() > offset) {
        return new Token_Head(this->getStartChar() - offset);
    } else {
        return new Token_Head();
    }
}

} // namespace inflection
