/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/RuntimeException.hpp>

namespace inflection::exception {

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

} // namespace inflection::exception
