/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/tokenizer/locale/fwd.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <vector>

class inflection::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor_AffixRecord final
{
private: /* package */
    ::std::map<char16_t, SemiticWordAndDelimiterTokenExtractor_AffixRecord> affixMap {  };
    bool isFinal {  };

public:
    SemiticWordAndDelimiterTokenExtractor_AffixRecord* addNextCharacter(char16_t ch);
    const SemiticWordAndDelimiterTokenExtractor_AffixRecord* getNextAffix(char16_t ch) const;

public: /* package */
    SemiticWordAndDelimiterTokenExtractor_AffixRecord();

private:
    friend class SemiticWordAndDelimiterTokenExtractor;
};
