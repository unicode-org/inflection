/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/Inflector.hpp>

#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <inflection/npc.hpp>
#include <string>
#include <map>

namespace inflection::dictionary {

const Inflector& Inflector::getInflector(const ::inflection::util::ULocale &locale) {
    const Inflector* result = npc(DictionaryMetaData::createDictionary(locale))->dictionary->inflector.get();
    if (result == nullptr) {
        throw ::inflection::exception::NullPointerException(inflection::util::StringViewUtils::to_u16string("Inflector not found for " + locale.getName()));
    }
    return *result;
}

Inflector::Inflector(inflection::util::MemoryMappedFile& memoryMappedFile, const ::std::u16string& sourcePath, const ::inflection::dictionary::DictionaryMetaData_MMappedDictionary &dictionary)
    : super()
    , mmappedDictionary(memoryMappedFile, sourcePath, dictionary)
{
}

Inflector::~Inflector()
{
}

std::optional<Inflector_InflectionPattern> Inflector::getInflectionPatternByName(std::u16string_view name) const
{
    auto searchResult = mmappedDictionary.getInflectionPatternIndexFromName(name);
    if (searchResult) {
        return mmappedDictionary.getInflectionPattern(*searchResult);
    }

    return {};
}

void Inflector::getInflectionPatternsForWord(std::u16string_view word, ::std::vector<Inflector_InflectionPattern> &inflectionPatterns) const {
    std::vector<int32_t> inflectionIdentifiers;
    if (mmappedDictionary.getInflectionPatternIdentifiers(inflectionIdentifiers, word)) {
        for (const auto identifier: inflectionIdentifiers) {
            inflectionPatterns.push_back(mmappedDictionary.getInflectionPattern(identifier));
        }
    }
}

::std::vector<Inflector_InflectionPattern> Inflector::getInflectionPatternsBySuffix(std::u16string_view suffix) const
{
    return mmappedDictionary.getInflectionPatternsFromSuffix(suffix);
}

} // namespace inflection::dictionary
