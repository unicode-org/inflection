/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/XMLParseException.hpp>

namespace morphuntion::exception {


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

} // namespace morphuntion::exception
