/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/ClassCastException.hpp>

namespace inflection::exception {

ClassCastException::ClassCastException()
    : super()
{
}

ClassCastException::ClassCastException(const std::u16string& message)
    : super(message)
{
}

ClassCastException::~ClassCastException()
{
}

} // namespace inflection::exception
