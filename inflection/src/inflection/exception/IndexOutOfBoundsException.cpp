/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/IndexOutOfBoundsException.hpp>

namespace inflection::exception {

IndexOutOfBoundsException::IndexOutOfBoundsException()
    : super()
{
}

IndexOutOfBoundsException::IndexOutOfBoundsException(const std::u16string& message)
    : super(message)
{
}

IndexOutOfBoundsException::~IndexOutOfBoundsException()
{
}

} // namespace inflection::exception
