/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/Exception.hpp>

namespace inflection::exception {

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

} // namespace inflection::exception
