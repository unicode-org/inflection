/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/IncompatibleVersionException.hpp>

namespace morphuntion::exception {

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

} // namespace morphuntion::exception
