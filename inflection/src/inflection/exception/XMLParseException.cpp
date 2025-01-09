/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/XMLParseException.hpp>

namespace inflection::exception {


XMLParseException::XMLParseException()
    : super()
{
}

XMLParseException::XMLParseException(const std::u16string& message)
    : super(message)
{
}

XMLParseException::~XMLParseException()
{
}

} // namespace inflection::exception
