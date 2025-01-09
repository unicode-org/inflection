/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/Token_Delim.hpp>

namespace inflection::tokenizer {

Token_Delim::Token_Delim(int32_t begin, int32_t end, std::u16string_view input, std::u16string_view clean)
    : super(begin, end, input, clean, false)
{
}

Token* Token_Delim::newInstance(int32_t offset) const
{
    return new Token_Delim(this->getStartChar() - offset, this->getEndChar() - offset, this->getValue(), this->getCleanValue());
}


} // namespace inflection
