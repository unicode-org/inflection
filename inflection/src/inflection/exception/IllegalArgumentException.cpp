/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/IllegalArgumentException.hpp>

namespace inflection::exception {

IllegalArgumentException::IllegalArgumentException()
    : super()
{
}

IllegalArgumentException::IllegalArgumentException(const std::u16string& message)
    : super(message)
{
}

IllegalArgumentException::~IllegalArgumentException()
{
}

} // namespace inflection::exception
