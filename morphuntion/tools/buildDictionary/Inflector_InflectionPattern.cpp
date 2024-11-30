/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include "Inflector_InflectionPattern.hpp"

#include <string>
#include <vector>
#include <algorithm>

Inflector_InflectionPattern::Inflector_InflectionPattern(::std::u16string_view identifier, int32_t frequency, int64_t partsOfSpeech, const ::std::vector<std::u16string>& lemmaSuffixes)
    : identifier(identifier)
    , frequency(frequency)
    , partsOfSpeech(partsOfSpeech)
    , lemmaSuffixes(lemmaSuffixes)
{
}

bool Inflector_InflectionPattern::operator<(const Inflector_InflectionPattern& other) const
{
    return (identifier < other.identifier);
}
