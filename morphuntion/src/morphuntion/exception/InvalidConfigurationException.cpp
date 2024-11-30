/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/InvalidConfigurationException.hpp>

namespace morphuntion::exception {

InvalidConfigurationException::InvalidConfigurationException()
    : super()
{
}

InvalidConfigurationException::InvalidConfigurationException(const std::u16string& message)
    : super(message)
{
}

InvalidConfigurationException::~InvalidConfigurationException()
{
}

} // namespace morphuntion::exception
