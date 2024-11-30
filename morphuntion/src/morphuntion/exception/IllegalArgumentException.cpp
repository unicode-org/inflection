/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/IllegalArgumentException.hpp>

namespace morphuntion::exception {

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

} // namespace morphuntion::exception
