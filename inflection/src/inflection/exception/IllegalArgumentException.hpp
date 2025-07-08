/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate an illegal argument to a function.
 * @details This is thrown in many places. It typically denotes that an argument to a function is in an incorrect format.
 * This can mean a wide variety of things, such as the wrong data structure being passed in, to having too many
 * elements in an array.
 */
class INFLECTION_CLASS_API inflection::exception::IllegalArgumentException
    : public inflection::exception::RuntimeException
{
public:
    /// @cond
    typedef ::inflection::exception::RuntimeException super;
    /// @endcond

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
