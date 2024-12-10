/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate an illegal argument to a function.
 * @details This is thrown in many places. It typically denotes that an argument to a function is in an incorrect format.
 * This can mean a wide variety of things, such as the wrong data structure being passed in, to having too many
 * elements in an array.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::IllegalArgumentException
    : public morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;

public:
    /**
     * Default constructor
     */
    IllegalArgumentException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit IllegalArgumentException(const std::u16string& message);
    /**
     * Destructor
     */
    ~IllegalArgumentException() override;
};
