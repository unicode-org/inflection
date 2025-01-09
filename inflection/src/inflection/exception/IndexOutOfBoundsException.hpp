/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate that an attempt to access an out of bounds index was made.
 * @details This can be thrown in many places.
 */
class INFLECTION_CLASS_API inflection::exception::IndexOutOfBoundsException
    : public inflection::exception::RuntimeException
{
public:
    typedef ::inflection::exception::RuntimeException super;

public:
    /**
     * Default constructor
     */
    IndexOutOfBoundsException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit IndexOutOfBoundsException(const std::u16string& message);
    /**
     * Destructor
     */
    ~IndexOutOfBoundsException() override;
};
