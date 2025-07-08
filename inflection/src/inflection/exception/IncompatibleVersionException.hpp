/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Thrown when a file loaded by Inflection has an incompatible version identifier.
 * @details This is typically thrown when loading Inflection's dictionary resources, as those resource versions must match
 * the versions Inflection was built with.
 */
class INFLECTION_CLASS_API inflection::exception::IncompatibleVersionException
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
    IncompatibleVersionException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit IncompatibleVersionException(const std::u16string& message);
    /**
     * Destructor
     */
    ~IncompatibleVersionException() override;
};
