/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/IllegalStateException.hpp>

namespace inflection::exception {

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

} // namespace inflection::exception
