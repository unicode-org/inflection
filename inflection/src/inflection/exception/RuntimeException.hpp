/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/Exception.hpp>

/**
 * @brief Used to indicate a generic exception occurring at runtime.
 * @details This class is meant to be subclassed by more specific exceptions, however it is possible that no other
 * exception exists that classifies the problem, so a generic RuntimeException will be thrown instead.
 */
class INFLECTION_CLASS_API inflection::exception::RuntimeException
    : public inflection::exception::Exception
{
public:
    /// @cond
    typedef ::inflection::exception::Exception super;
    /// @endcond

public:
    /**
     * Default constructor
     */
    RuntimeException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit RuntimeException(const std::u16string& message);
    /**
     * Destructor
     */
    ~RuntimeException() override;
};
