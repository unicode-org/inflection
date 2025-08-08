/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#include <inflection/tokenizer/locale/DravidianWordAndDelimiterTokenExtractor_AffixRecord.hpp>

namespace inflection::tokenizer::locale {

DravidianWordAndDelimiterTokenExtractor_AffixRecord::DravidianWordAndDelimiterTokenExtractor_AffixRecord()
{
}

DravidianWordAndDelimiterTokenExtractor_AffixRecord* DravidianWordAndDelimiterTokenExtractor_AffixRecord::addNextCharacter(char16_t ch)
{
    auto affixRecordEntry = affixMap.find(ch);
    if (affixRecordEntry == affixMap.end()) {
        auto entry = affixMap.emplace(std::pair<char16_t, DravidianWordAndDelimiterTokenExtractor_AffixRecord>(ch, {}));
        return &entry.first->second;
    }
    return &affixRecordEntry->second;
}

const DravidianWordAndDelimiterTokenExtractor_AffixRecord* DravidianWordAndDelimiterTokenExtractor_AffixRecord::getNextAffix(char16_t ch) const
{
    auto result = affixMap.find(ch);
    if (result != affixMap.end()) {
        return &result->second;
    }
    return nullptr;
}

} // namespace inflection::tokenizer::locale