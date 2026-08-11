/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/fwd.hpp>
#include <inflection/Object.hpp>

/**
 * @brief A continuous subset of characters within a character set
 * @details This class is typically used to represent a continuous subset of characters within
 * the unicode character set. It is subclassed by Token.
 */
class INFLECTION_INTERNAL_API inflection::tokenizer::CharRange
    : public virtual ::inflection::Object
{
public:
    /**
     * @return Returns the UTF-16 code unit index in the string for the first character.
     */
    virtual int32_t getStartChar() const = 0;
    /**
     * @return Returns the UTF-16 code unit index after the last character.
     */
    virtual int32_t getEndChar() const = 0;
    /**
     * Destructor
     */
    ~CharRange() override;
};
