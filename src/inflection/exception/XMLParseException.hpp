/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/Exception.hpp>

/**
 * @brief Used to indicate an error parsing XML documents.
 * @details This type of exception is only used when running the XML tools that aid in creating the resources used by
 * Inflection. It will never be thrown during runtime.
 */
class INFLECTION_CLASS_API inflection::exception::XMLParseException
    : public inflection::exception::Exception
{
public:
    /// @cond
    typedef ::inflection::exception::Exception super;
    /// @endcond


public:
    XMLParseException();
    /**
     * Construct an XMLParseException with a message.
     * @param message The debugging message of this XML parse exception.
     */
    explicit XMLParseException(const std::u16string& message);
    ~XMLParseException() override;
};
