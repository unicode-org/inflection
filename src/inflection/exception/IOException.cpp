/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/IOException.hpp>

namespace inflection::exception {

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

} // namespace inflection::exception
