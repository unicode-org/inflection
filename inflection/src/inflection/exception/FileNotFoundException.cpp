/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/FileNotFoundException.hpp>

namespace inflection::exception {


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

} // namespace inflection::exception
