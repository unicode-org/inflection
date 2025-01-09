/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/Object.hpp>

/**
 * @brief The base interface for all concepts that can be formatted to a SpeakableString.
 */
class INFLECTION_CLASS_API inflection::dialog::SpeakableConcept
    : public virtual ::inflection::Object
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
