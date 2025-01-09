/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
#include "Inflector_Inflection.hpp"

#include <string>

Inflector_Inflection::Inflector_Inflection(::std::u16string_view suffix, int64_t grammemes)
    : suffix(suffix)
    , grammemes(grammemes)
{
}
