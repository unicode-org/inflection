/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/Object.hpp>

/**
 * @brief The base interface for all concepts that can be formatted to a SpeakableString.
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::SpeakableConcept
    : public virtual ::morphuntion::Object
{
public:
    /**
     * Returns the concept as a SpeakableString.
     * @return The concept represented as a SpeakableString.
     */
    virtual SpeakableString* toSpeakableString() const = 0;
    /**
     * Destructor
     */
    ~SpeakableConcept() override;
};
