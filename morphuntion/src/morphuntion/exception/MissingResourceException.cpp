/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/MissingResourceException.hpp>

namespace morphuntion::exception {

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

} // namespace morphuntion::exception
