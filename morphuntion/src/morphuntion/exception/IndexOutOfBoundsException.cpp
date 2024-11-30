/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/IndexOutOfBoundsException.hpp>

namespace morphuntion::exception {

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

} // namespace morphuntion::exception
