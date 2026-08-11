/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/Token.hpp>

#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Tail.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/Token_Delim.hpp>
#include <inflection/util/StringUtils.hpp>
#include <unicode/uchar.h>

namespace inflection::tokenizer {

Token::~Token() {
}

Token::Token(int32_t startChar, int32_t endChar, std::u16string_view value, std::u16string_view cleanValue, bool significant)
    : super()
    , startChar(startChar)
    , endChar(endChar)
    , value(value)
    , cleanValue(cleanValue)
    , significant(significant)
    , whitespace(calculateWhiteSpace())
{
}

std::u16string_view getTypeStr(const Token* token)
{
    ::std::u16string tokenType;
    if (dynamic_cast< const Token_Word* >(token) != nullptr) {
        return u"Word";
    }
    else if (dynamic_cast< const Token_Tail* >(token) != nullptr) {
        return u"Tail";
    }
    else if (dynamic_cast< const Token_Head* >(token) != nullptr) {
        return u"Head";
    }
    else if (dynamic_cast< const Token_Delim* >(token) != nullptr) {
        return u"Delim";
    }
    return u"Token"; // Not a valid value.
}

bool Token::operator==(const Token& rhs) const
{
    if (this == &rhs) {
        return true;
    }
    if (getTypeStr(this) != getTypeStr(&rhs)) {
        return false;
    }
    return getLength() == rhs.getLength() && isSignificant() == rhs.isSignificant() && isHead() == rhs.isHead() && isTail() == rhs.isTail() && isWhitespace() == rhs.isWhitespace() && getCleanValue() == rhs.getCleanValue() && getValue() == rhs.getValue();
}

std::size_t Token::operator()(const Token& token) const noexcept
{
    return std::hash<std::u16string>()(token.value);
}

bool Token::calculateWhiteSpace() const
{
    int32_t length = int32_t(value.length());
    if (length == 0) {
        return false;
    }
    for (int32_t idx = 0; idx < length; idx++) {
        if (!(bool)u_isUWhiteSpace(value[idx])) {
            return false;
        }
    }
    return true;
}

int32_t Token::getStartChar() const
{
    return startChar;
}

int32_t Token::getEndChar() const
{
    return endChar;
}

int32_t Token::getLength() const
{
    return endChar - startChar;
}

const ::std::u16string& Token::getValue() const
{
    return value;
}

const ::std::u16string& Token::getCleanValue() const
{
    return cleanValue;
}

Token* Token::getPrevious() const
{
    return previous;
}

void Token::setPrevious(Token* previous)
{
    this->previous = previous;
}

Token* Token::getNext() const
{
    return next;
}

void Token::setNext(Token* next)
{
    this->next = next;
}

bool Token::isSignificant() const
{
    return this->significant;
}

bool Token::isWhitespace() const
{
    return this->whitespace;
}

::std::u16string Token::toString() const
{
    return std::u16string(getTypeStr(this)) + ::std::u16string(u"{begin=") + util::StringUtils::to_u16string(startChar)
        + ::std::u16string(u", end=") + util::StringUtils::to_u16string(endChar)
        + ::std::u16string(u", value='") + value
        + ::std::u16string(u"', cleanValue='") + cleanValue
        + ::std::u16string(u"'}");
}

bool Token::isHead() const
{
    return dynamic_cast< const Token_Head* >(this) != nullptr;
}

bool Token::isTail() const
{
    return dynamic_cast< const Token_Tail* >(this) != nullptr;
}

} // namespace inflection
