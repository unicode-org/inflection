/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/Token_Word.hpp>

namespace inflection::tokenizer {

Token_Word::Token_Word(int32_t begin, int32_t end, std::u16string_view input, std::u16string_view clean, bool significant)
    : super(begin, end, input, clean, significant)
{
}

Token* Token_Word::newInstance(int32_t offset) const
{
    return new Token_Word(this->getStartChar() - offset, this->getEndChar() - offset, this->getValue(), this->getCleanValue(), this->isSignificant());
}

} // namespace inflection
