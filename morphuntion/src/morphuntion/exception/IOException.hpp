/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate a general IO error.
 * @details This exception type is used to indicate a general IO error, such as failing to read a file.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::IOException
    : public morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;


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
