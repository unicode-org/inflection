/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/StringIndexOutOfBoundsException.hpp>

#include <inflection/util/StringUtils.hpp>

namespace inflection::exception {

StringIndexOutOfBoundsException::StringIndexOutOfBoundsException()
    : super()
{
}

StringIndexOutOfBoundsException::StringIndexOutOfBoundsException(int32_t index)
    : super(u"StringIndexOutOfBoundsException at " + ::inflection::util::StringUtils::to_u16string(index))
{
}

StringIndexOutOfBoundsException::StringIndexOutOfBoundsException(const std::u16string& message)
    : super(message)
{
}

StringIndexOutOfBoundsException::~StringIndexOutOfBoundsException()
{
}

} // namespace inflection::exception
