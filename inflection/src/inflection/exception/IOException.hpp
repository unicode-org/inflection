/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate a general IO error.
 * @details This exception type is used to indicate a general IO error, such as failing to read a file.
 */
class INFLECTION_CLASS_API inflection::exception::IOException
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
    IOException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit IOException(const std::u16string& message);
    /**
     * Destructor
     */
    ~IOException() override;
};
