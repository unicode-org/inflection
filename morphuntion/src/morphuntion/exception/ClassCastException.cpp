/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/ClassCastException.hpp>

namespace morphuntion::exception {

ClassCastException::ClassCastException()
    : super()
{
}

ClassCastException::ClassCastException(const std::u16string& message)
    : super(message)
{
}

ClassCastException::~ClassCastException()
{
}

} // namespace morphuntion::exception
