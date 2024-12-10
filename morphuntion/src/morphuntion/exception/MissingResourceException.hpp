/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/exception/fwd.hpp>
#include <morphuntion/exception/RuntimeException.hpp>

/**
 * @brief Used to indicate a resource that is required by Morphuntion is missing.
 * @details This is typically thrown by factories when constructing rules for inflection or tokenization. One such
 * language that can throw this is Hebrew.
 *
 * This exception type is meant to be used with morphuntion::resources::DataResource. And indicates the absence of a
 * specific key in a resource file.
 */
class MORPHUNTION_CLASS_API morphuntion::exception::MissingResourceException
    : public morphuntion::exception::RuntimeException
{
public:
    typedef ::morphuntion::exception::RuntimeException super;

private:
    ::std::u16string className;
    ::std::u16string key;

public:
    /**
     * Constructor.
     *
     * @param message The message to add to the exception.
     * @param className The class name of the object throwing this exception.
     * @param key The key being looked up by morphuntion::resources::DataResource::getString().
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
     * Returns the name of the key used during morphuntion::resources::DataResource::getString() lookup.
     * @return The name of the key.
     */
    virtual const ::std::u16string& getKey() const noexcept;
};
