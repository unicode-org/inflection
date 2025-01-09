/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include "Inflector_Inflection.hpp"
#include "inflection/Object.hpp"
#include <string>
#include <vector>

class Inflector_InflectionPattern final
{
protected:
    ::std::u16string identifier {  };
    int32_t frequency {  };
    int64_t partsOfSpeech {  };
    ::std::vector<std::u16string> lemmaSuffixes {  };
    ::std::vector<Inflector_Inflection> inflectionForGrammeme {  };

public:
    virtual bool operator<(const Inflector_InflectionPattern& other) const;

protected:
    Inflector_InflectionPattern(::std::u16string_view identifier, int32_t frequency, int64_t partsOfSpeech, const ::std::vector<std::u16string>& lemmaSuffixes);

private:
    friend class Inflector;
    friend class Inflector_InflectionDictionary;
    friend class Inflector_Suffix;
    friend class Inflector_Inflection;
    friend class InflectionDictionary;
};
