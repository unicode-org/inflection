/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate a resource that is required by Inflection is missing.
 * @details This is typically thrown by factories when constructing rules for inflection or tokenization. One such
 * language that can throw this is Hebrew.
 *
 * This exception type is meant to be used with inflection::resources::DataResource. And indicates the absence of a
 * specific key in a resource file.
 */
class INFLECTION_CLASS_API inflection::exception::MissingResourceException
    : public inflection::exception::RuntimeException
{
public:
    /// @cond
    typedef ::inflection::exception::RuntimeException super;
    /// @endcond

private:
    ::std::u16string className;
    ::std::u16string key;

public:
    /**
     * Constructor.
     *
     * @param message The message to add to the exception.
     * @param className The class name of the object throwing this exception.
     * @param key The key being looked up by inflection::resources::DataResource::getString().
     */
    MissingResourceException(const std::u16string& message, const std::u16string& className, const std::u16string& key);
    /**
     * Destructor
     */
    ~MissingResourceException() override;

    /**
     * Returns the class name of the object that threw this exception.
     * @return The class name of the object that threw this exception.
     */
    virtual const ::std::u16string& getClassName() const noexcept;
    /**
     * Returns the name of the key used during inflection::resources::DataResource::getString() lookup.
     * @return The name of the key.
     */
    virtual const ::std::u16string& getKey() const noexcept;
};
