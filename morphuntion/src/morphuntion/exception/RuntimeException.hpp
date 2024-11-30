/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/Exception.hpp>

/**
 * @brief Used to indicate a generic exception occurring at runtime.
 * @details This class is meant to be subclassed by more specific exceptions, however it is possible that no other
 * exception exists that classifies the problem, so a generic RuntimeException will be thrown instead.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::RuntimeException
    : public morphuntion::exception::Exception
{
public:
    typedef ::morphuntion::exception::Exception super;

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
