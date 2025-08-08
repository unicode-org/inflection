/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#pragma once

#include <inflection/tokenizer/locale/fwd.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <vector>

namespace inflection::tokenizer::locale {

class DravidianWordAndDelimiterTokenExtractor_AffixRecord {
public:
    DravidianWordAndDelimiterTokenExtractor_AffixRecord();

    DravidianWordAndDelimiterTokenExtractor_AffixRecord* addNextCharacter(char16_t ch);

    const DravidianWordAndDelimiterTokenExtractor_AffixRecord* getNextAffix(char16_t ch) const;

private:
    std::unordered_map<char16_t, DravidianWordAndDelimiterTokenExtractor_AffixRecord> affixMap;
};

} // namespace inflection::tokenizer::locale