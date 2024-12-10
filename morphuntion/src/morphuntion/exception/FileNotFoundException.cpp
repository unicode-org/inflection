/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/FileNotFoundException.hpp>

namespace morphuntion::exception {


FileNotFoundException::FileNotFoundException()
    : super()
{
}

FileNotFoundException::FileNotFoundException(const std::u16string& message)
    : super(message)
{
}

FileNotFoundException::~FileNotFoundException()
{
}

} // namespace morphuntion::exception
