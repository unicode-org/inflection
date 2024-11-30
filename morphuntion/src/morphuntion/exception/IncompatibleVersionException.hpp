/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Thrown when a file loaded by Morphuntion has an incompatible version identifier.
 * @details This is typically thrown when loading Morphuntion's dictionary resources, as those resource versions must match
 * the versions Morphuntion was built with.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::IncompatibleVersionException
    : public ::morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;

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
