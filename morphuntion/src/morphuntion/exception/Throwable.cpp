/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */

#include <morphuntion/exception/Throwable.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <typeinfo>

namespace morphuntion::exception {

Throwable::Throwable()
{
}

Throwable::Throwable(const std::u16string& message)
    : message(message)
{
    ::morphuntion::util::StringViewUtils::convert(&whatStr, message);
}

Throwable::~Throwable()
{
}

const char *Throwable::what() const noexcept {
    return whatStr.c_str();
}

const ::std::u16string& Throwable::getMessage() const noexcept {
    return message;
}

} // namespace morphuntion::exception
