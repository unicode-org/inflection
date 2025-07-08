/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate that a datastructure is in an unknown or bad state.
 * @details This is typically thrown during tokenization or when constructing tries. It indicates that some piece of
 * data is incorrect or is missing, and that continuing operation would cause the process to hang or crash (or at the
 * least be unknown).
 */
class INFLECTION_CLASS_API inflection::exception::IllegalStateException
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
    IllegalStateException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit IllegalStateException(const std::u16string& message);
    /**
     * Destructor
     */
    ~IllegalStateException() override;
};
