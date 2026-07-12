/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/Exception.hpp>

/**
 * @brief Used to indicate excess complexity of an operation (typically recursion).
 * @details This is typically thrown during Finnish Token attribution, especially when decompounding words.
 */
class INFLECTION_CLASS_API inflection::exception::ExcessComplexityException
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
    ExcessComplexityException();
    /**
     * Destructor
     */
    ~ExcessComplexityException() override;
};
