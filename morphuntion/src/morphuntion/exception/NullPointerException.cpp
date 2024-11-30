/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/NullPointerException.hpp>

namespace morphuntion::exception {


NullPointerException::NullPointerException()
    : super(u"NullPointerException")
{
}

NullPointerException::NullPointerException(const std::u16string& message)
    : super(message)
{
}

NullPointerException::~NullPointerException()
{
}

} // namespace morphuntion::exception
