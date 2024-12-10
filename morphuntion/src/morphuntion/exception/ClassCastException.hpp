/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate an error casting from one class to other.
 * @details This is typically thrown during reflection of AttributeImpl.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::ClassCastException
    : public morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;

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
