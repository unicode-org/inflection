/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <inflection/util/StringUtils.hpp>

namespace inflection::exception {

IndexOutOfBoundsException::IndexOutOfBoundsException()
    : super()
{
}

IndexOutOfBoundsException::IndexOutOfBoundsException(int32_t index)
    : super(u"IndexOutOfBoundsException at " + ::inflection::util::StringUtils::to_u16string(index))
{
}

IndexOutOfBoundsException::IndexOutOfBoundsException(const std::u16string& message)
    : super(message)
{
}

IndexOutOfBoundsException::~IndexOutOfBoundsException()
{
}

} // namespace inflection::exception
