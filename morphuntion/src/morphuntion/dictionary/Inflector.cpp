/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dictionary/Inflector.hpp>

#include <morphuntion/dictionary/Inflector_InflectionPattern.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/dictionary/DictionaryMetaData_MMappedDictionary.hpp>
#include <morphuntion/npc.hpp>
#include <string>
#include <map>

namespace morphuntion::dictionary {

const Inflector& Inflector::getInflector(const ::morphuntion::util::ULocale &locale) {
    const Inflector* result = npc(npc(DictionaryMetaData::createDictionary(locale))->dictionary)->inflector;
    if (result == nullptr) {
        throw ::morphuntion::exception::NullPointerException(morphuntion::util::StringViewUtils::to_u16string("Inflector not found for " + locale.getName()));
    }
    return *result;
}

Inflector::Inflector(morphuntion::util::MemoryMappedFile& memoryMappedFile, const ::std::u16string& sourcePath, const ::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary &dictionary)
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

} // namespace morphuntion::dictionary
