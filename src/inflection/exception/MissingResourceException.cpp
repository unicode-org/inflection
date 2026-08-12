/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/MissingResourceException.hpp>

namespace inflection::exception {

MissingResourceException::MissingResourceException(const std::u16string& message, const std::u16string& className, const std::u16string& key)
    : super(message)
    , className(className)
    , key(key)
{
}

MissingResourceException::~MissingResourceException()
{
}

const ::std::u16string&
MissingResourceException::getClassName() const noexcept
{
    return className;
}

const ::std::u16string&
MissingResourceException::getKey() const noexcept
{
    return key;
}

} // namespace inflection::exception
