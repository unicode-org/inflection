/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate that a datastructure is in an unknown or bad state.
 * @details This is typically thrown during tokenization or when constructing tries. It indicates that some piece of
 * data is incorrect or is missing, and that continuing operation would cause the process to hang or crash (or at the
 * least be unknown).
 */
class MORPHUNTION_CLASS_API morphuntion::exception::IllegalStateException
    : public morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;

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
