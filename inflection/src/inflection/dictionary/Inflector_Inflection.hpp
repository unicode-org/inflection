/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <inflection/dictionary/metadata/StringContainer.hpp>
#include <string>
#include <vector>
#include <map>

class inflection::dictionary::Inflector_Inflection final
{
private:
    const Inflector_InflectionPattern& inflectionPattern;

    const int32_t suffixId {  };
    const int64_t grammemes {  };

public:
    ::std::u16string inflect(const ::std::u16string& lemma) const;
    ::std::u16string lemmatize(const ::std::u16string& noun) const;

    int64_t getGrammemes() const;

    ::std::u16string getSuffix() const;
    const Inflector_InflectionPattern * getInflectionPattern() const;

public: /* package */
    Inflector_Inflection(const Inflector_InflectionPattern& inflectionPattern, int32_t suffixId, int64_t grammemes);

private:
    friend class Inflector;
    friend class Inflector_MMappedDictionary;
    friend class Inflector_InflectionPattern;
};
