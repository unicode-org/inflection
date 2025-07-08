/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/IOException.hpp>

/**
 * @brief Used to indicate a missing file.
 * @details This is thrown in many places, such as by factories for tokenization, analysis, and by the resource loader.
 */
class INFLECTION_CLASS_API inflection::exception::FileNotFoundException
    : public inflection::exception::IOException
{
public:
    /// @cond
    typedef ::inflection::exception::IOException super;
    /// @endcond


public:
    /**
     * Default constructor
     */
    FileNotFoundException();
    /**
     * Construct an exception with an informative message for debugging purposes.
     */
    explicit FileNotFoundException(const std::u16string& message);
    /**
     * Destructor
     */
    ~FileNotFoundException() override;
};
