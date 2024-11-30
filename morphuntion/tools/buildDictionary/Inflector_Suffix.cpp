/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "Inflector_Suffix.hpp"

#include <string>

Inflector_Suffix::Inflector_Suffix(::std::u16string_view suffix, int32_t frequency)
    : id(suffix)
    , frequency(frequency)
{
}

bool Inflector_Suffix::operator<(const Inflector_Suffix& other) const
{
    if (frequency != other.frequency) {
        return (frequency > other.frequency);
    }
    return (id < other.id);
}
