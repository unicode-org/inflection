/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <exception>
#include <string>

/**
 * @brief Provides basic capabilities to @ref Exception "Exception"s in Inflection.
 * @details This class is meant to be subclassed by Exception. It provides a common interface for throwable exceptions
 * by inheriting specifically from <code>std::exception</code>. It provides two common access methods for data in
 * subclasses - <code>Throwable::what()</code> and <code>Throwable::getMessage()</code>, where what() returns a C-type
 * string (<code>const char*</code>) and getMessage() returns a <code>std::u16string</code>. The messages returned by
 * these two methods are typically identical, except for the fact that some characters may be lost in conversion from
 * UTF16 → C-type string.
 */
class INFLECTION_CLASS_API inflection::exception::Throwable
    : public ::std::exception
{
private:
    ::std::u16string message;
    ::std::string whatStr;

public:
    /**
     * Default constructor
     */
    Throwable();
    /**
     * Constructs a new Throwable with the input message. The message will be stored as both a <code>std::u16string</code>
     * and a C-type string.
     * @param message The input message.
     */
    explicit Throwable(const std::u16string& message);
    /**
     * Destructor
     */
    ~Throwable() override;

public:
    /**
     * Returns the Throwable message as a C-type string.
     * @return The Throwable message as a C-type string.
     */
    const char* what() const noexcept override;
    /**
     * Returns the Throwable message as a <code>std::u16string</code>.
     * @return The Throwable message as a <code>std::u16string</code>.
     */
    virtual const ::std::u16string& getMessage() const noexcept;

};
