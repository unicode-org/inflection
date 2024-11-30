/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/IllegalStateException.hpp>

namespace morphuntion::exception {

IllegalStateException::IllegalStateException()
    : super()
{
}

IllegalStateException::IllegalStateException(const std::u16string& message)
    : super(message)
{
}

IllegalStateException::~IllegalStateException()
{
}

} // namespace morphuntion::exception
