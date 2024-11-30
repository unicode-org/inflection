/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <string>
#include <vector>
#include <map>

class Inflector_Inflection final
{
private:
    ::std::u16string suffix {  };
    int64_t grammemes {  };

public:
    Inflector_Inflection(::std::u16string_view suffix, int64_t grammemes);

private:
    friend class InflectionDictionary;
    friend class Inflector;
    friend class Inflector_InflectionPattern;
};
