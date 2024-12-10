/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/IndexOutOfBoundsException.hpp>

/**
 * @brief Used to indicate when an attempt to access a character in <code>std::u16string</code> is out of bounds.
 * @details This type of exception is only thrown in morphuntion::util::StringViewUtils::codePointAt().
 */
class MORPHUNTION_CLASS_API morphuntion::exception::StringIndexOutOfBoundsException
    : public morphuntion::exception::IndexOutOfBoundsException
{
public:
    typedef ::morphuntion::exception::IndexOutOfBoundsException super;

public:
    StringIndexOutOfBoundsException();
    /**
     * Constructs a new StringIndexOutOfBoundsException with the index that caused the exception to occur.
     * @param index The index that caused the exception to occur.
     */
    explicit StringIndexOutOfBoundsException(int32_t index);
    /**
     * Constructs a new StringIndexOutOfBoundsException with a specified message.
     * @param message The message.
     */
    explicit StringIndexOutOfBoundsException(const std::u16string& message);
    /**
     * Destructor
     */
    ~StringIndexOutOfBoundsException() override;
};
