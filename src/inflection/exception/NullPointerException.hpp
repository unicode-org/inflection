/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate a pointer is null after a null pointer check.
 * @details This exception can be thrown in **many** places in Inflection. Usually this exception is thrown when this
 * library gets into a state where it's expecting a non-null pointer and it has a null pointer instead.
 */
class INFLECTION_CLASS_API inflection::exception::NullPointerException
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
    NullPointerException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit NullPointerException(const std::u16string& message);
    /**
     * Destructor
     */
    ~NullPointerException() override;
};
