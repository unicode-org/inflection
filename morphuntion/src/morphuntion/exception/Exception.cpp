/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/Exception.hpp>

namespace morphuntion::exception {

Exception::Exception()
    : super()
{
}

Exception::Exception(const std::u16string& message)
    : super(message)
{
}

Exception::~Exception()
{
}

} // namespace morphuntion::exception
