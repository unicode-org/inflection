/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/Throwable.hpp>

/**
 * @brief Base exception class for all exceptions in Inflection.
 * @details This class defines _exceptions_ in Inflection. It provides similar syntax and semantics to Java's exceptions
 * (although restricted by the limitations of C++), and provides two basic methods to be overridden by subclassing.
 * It is preferable to pass in a message explaining the cause of the exception, although many subclasses may add their
 * own messages to the passed in message.
 *
 * Since all @ref Exception "Exception"s in Inflection inherit from <code>std::exception</code> you may simply
 * <code>throw</code> the exception like so: <code>throw inflection::exception::Exception(u"Uh oh spaghettio");</code>
 */
class INFLECTION_CLASS_API inflection::exception::Exception
    : public inflection::exception::Throwable
{
public:
    /// @cond
    typedef ::inflection::exception::Throwable super;
    /// @endcond

public:
    /**
     * Creates a new Exception.
     */
    Exception();
    /**
     * Creates a new Exception with a message to be printed with the exception.
     * @param message The message to be added to the exception.
     */
    explicit Exception(const std::u16string& message);
    /**
     * Destructor
     */
    ~Exception() override;
};
