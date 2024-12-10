/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/fwd.hpp>
#include <morphuntion/Object.hpp>

/**
 * @brief A continuous subset of characters within a character set
 * @details This class is typically used to represent a continuous subset of characters within
 * the unicode character set. It is subclassed by Token.
 */
class MORPHUNTION_INTERNAL_API morphuntion::tokenizer::CharRange
    : public virtual ::morphuntion::Object
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
