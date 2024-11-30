/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/RuntimeException.hpp>

namespace morphuntion::exception {

RuntimeException::RuntimeException()
    : super()
{
}

RuntimeException::RuntimeException(const std::u16string& message)
    : super(message)
{
}

RuntimeException::~RuntimeException()
{
}

} // namespace morphuntion::exception
