/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/tokenizer/locale/SemiticWordAndDelimiterTokenExtractor_AffixRecord.hpp>

namespace morphuntion::tokenizer::locale {

SemiticWordAndDelimiterTokenExtractor_AffixRecord::SemiticWordAndDelimiterTokenExtractor_AffixRecord()
{
}

SemiticWordAndDelimiterTokenExtractor_AffixRecord* SemiticWordAndDelimiterTokenExtractor_AffixRecord::addNextCharacter(char16_t ch)
{
    auto affixRecordEntry = affixMap.find(ch);
    if (affixRecordEntry == affixMap.end()) {
        auto entry = affixMap.emplace(std::pair<char16_t, SemiticWordAndDelimiterTokenExtractor_AffixRecord>(ch, {}));
        return &entry.first->second;
    }
    return &affixRecordEntry->second;
}

const SemiticWordAndDelimiterTokenExtractor_AffixRecord* SemiticWordAndDelimiterTokenExtractor_AffixRecord::getNextAffix(char16_t ch) const
{
    auto result = affixMap.find(ch);
    if (result != affixMap.end()) {
        return &result->second;
    }
    return nullptr;
}

} // namespace morphuntion::tokenizer
