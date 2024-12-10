/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include "Inflector_InflectionPattern.hpp"
#include "Inflector_Suffix.hpp"
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <set>

class Inflector_InflectionDictionary final
{
protected:
    bool forLemmatization {  };
    ::std::vector<Inflector_InflectionPattern> inflectionPatterns {  };
    ::std::map<std::u16string, int32_t> inflectionIdToPatternIdx {  };
    ::std::map<std::u16string, ::std::set<Inflector_Suffix>> suffixPatterns {  };

protected:
    explicit Inflector_InflectionDictionary(const ::morphuntion::util::ULocale& locale);


private:
    friend class Inflector;
    friend class Inflector_Suffix;
    friend class Inflector_Inflection;
    friend class Inflector_InflectionPattern;
    friend class InflectionDictionary;
};
