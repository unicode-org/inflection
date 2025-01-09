/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/IncompatibleVersionException.hpp>

namespace inflection::exception {

IncompatibleVersionException::IncompatibleVersionException()
    : super()
{
}

IncompatibleVersionException::IncompatibleVersionException(const std::u16string& message)
    : super(message)
{
}

IncompatibleVersionException::~IncompatibleVersionException()
{
}

} // namespace inflection::exception
