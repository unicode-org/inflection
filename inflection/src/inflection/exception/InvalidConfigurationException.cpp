/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/InvalidConfigurationException.hpp>

namespace inflection::exception {

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

} // namespace inflection::exception
