/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/Exception.hpp>

/**
 * @brief Used to indicate an error parsing XML documents.
 * @details This type of exception is only used when running the XML tools that aid in creating the resources used by
 * Morphuntion. It will never be thrown during runtime.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::XMLParseException
    : public morphuntion::exception::Exception
{
public:
    typedef ::morphuntion::exception::Exception super;


public:
    XMLParseException();
    explicit XMLParseException(const std::u16string& message);
    ~XMLParseException() override;
};
