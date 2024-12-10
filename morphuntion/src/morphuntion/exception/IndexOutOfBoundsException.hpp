/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate that an attempt to access an out of bounds index was made.
 * @details This can be thrown in many places.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::IndexOutOfBoundsException
    : public morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;

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
