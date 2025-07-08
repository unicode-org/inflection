/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate an error casting from one class to other.
 * @details This is typically thrown during reflection of AttributeImpl.
 */
class INFLECTION_CLASS_API inflection::exception::ClassCastException
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
    ClassCastException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit ClassCastException(const std::u16string& message);
    /**
     * Destructor
     */
    ~ClassCastException() override;
};
