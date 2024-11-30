/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/IOException.hpp>

namespace morphuntion::exception {

IOException::IOException()
    : super()
{
}

IOException::IOException(const std::u16string& message)
    : super(message)
{
}

IOException::~IOException()
{
}

} // namespace morphuntion::exception
