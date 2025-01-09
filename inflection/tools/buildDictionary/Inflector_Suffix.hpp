/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include "inflection/Object.hpp"
#include <string>

class Inflector_Suffix final
{
private:
    ::std::u16string id {  };
    int32_t frequency {  };

protected:
    Inflector_Suffix(::std::u16string_view suffix, int32_t frequency);


public:
    bool operator<(const Inflector_Suffix& other) const;

private:
    friend class Inflector;
    friend class Inflector_InflectionDictionary;
    friend class Inflector_Inflection;
    friend class Inflector_InflectionPattern;
    friend class InflectionDictionary;
};
