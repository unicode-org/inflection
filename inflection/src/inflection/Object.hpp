/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/fwd.hpp>
#include <string>

/**
 * @brief Inflection's root object.
 * @details This class provides the most basic capabilities to classes in Inflection. Almost all other classes inherit from
 * Object.
 */
class INFLECTION_CLASS_API inflection::Object {
public:
    /**
     * Returns a string representation of the Object. Classes may override the default behavior, which is to print the
     * typeid (class name) of the calling inheritor.
     *
     * @return A string representation of the Object.
     */
    virtual ::std::u16string toString() const;
    /**
     * Destructor.
     */
    virtual ~Object();
};
