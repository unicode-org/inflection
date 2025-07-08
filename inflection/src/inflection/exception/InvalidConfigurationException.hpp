/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate that a loaded configuration is invalid.
 * @details This is typically thrown by StringTokenizerBridge or germanic token extractors when there are missing files.
 * It can also be thrown by the transliterators when it is unable to load the transliteration map.
 */
class INFLECTION_CLASS_API inflection::exception::InvalidConfigurationException
    : public ::inflection::exception::RuntimeException
{
public:
    /// @cond
    typedef ::inflection::exception::RuntimeException super;
    /// @endcond

public:
    /**
     * Default constructor
     */
    InvalidConfigurationException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit InvalidConfigurationException(const std::u16string& message);
    /**
     * Destructor
     */
    ~InvalidConfigurationException() override;
};
