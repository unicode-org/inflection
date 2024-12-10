/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate that a loaded configuration is invalid.
 * @details This is typically thrown by StringTokenizerBridge or germanic token extractors when there are missing files.
 * It can also be thrown by the transliterators when it is unable to load the transliteration map.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::InvalidConfigurationException
    : public ::morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;

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
