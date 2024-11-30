/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/fwd.hpp>
#include <string>

/**
 * @brief Morphuntion's root object.
 * @details This class provides the most basic capabilities to classes in Morphuntion. Almost all other classes inherit from
 * Object.
 */
class MORPHUNTION_CLASS_API morphuntion::Object {
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
