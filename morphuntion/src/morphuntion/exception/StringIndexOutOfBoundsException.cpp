/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/StringIndexOutOfBoundsException.hpp>

#include <morphuntion/util/StringUtils.hpp>

namespace morphuntion::exception {

StringIndexOutOfBoundsException::StringIndexOutOfBoundsException()
    : super()
{
}

StringIndexOutOfBoundsException::StringIndexOutOfBoundsException(int32_t index)
    : super(u"StringIndexOutOfBoundsException at " + ::morphuntion::util::StringUtils::to_u16string(index))
{
}

StringIndexOutOfBoundsException::StringIndexOutOfBoundsException(const std::u16string& message)
    : super(message)
{
}

StringIndexOutOfBoundsException::~StringIndexOutOfBoundsException()
{
}

} // namespace morphuntion::exception
