/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/tokenizer/locale/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <vector>

class morphuntion::tokenizer::locale::SemiticWordAndDelimiterTokenExtractor_AffixRecord final
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
