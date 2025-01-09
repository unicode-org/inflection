/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */

#include <inflection/exception/Throwable.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <typeinfo>

namespace inflection::exception {

Throwable::Throwable()
{
}

Throwable::Throwable(const std::u16string& message)
    : message(message)
{
    ::inflection::util::StringViewUtils::convert(&whatStr, message);
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

} // namespace inflection::exception
