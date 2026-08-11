/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 4/14/22.
//
#include <inflection/analysis/DictionaryExposableMorphology.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/Logger.hpp>

namespace inflection::analysis {

DictionaryExposableMorphology::InflectionGrammemes::InflectionGrammemes(int64_t grammemes, std::optional<::inflection::dictionary::Inflector_Inflection> inflection)
    : grammemes(grammemes)
    , inflection(inflection)
{
}

std::u16string DictionaryExposableMorphology::InflectionGrammemes::getDescription(const ::inflection::dictionary::DictionaryMetaData &dictionary) const {
    return std::u16string(u"Inflection[")
        + (inflection.has_value()
            ? std::u16string(u"pattern-name = ").append(inflection->getInflectionPattern().getIdentifier())
                .append(u", suffix = \"").append(inflection->getSuffix()).append(u"\"")
            : std::u16string())
        + u"], grammemes["
        + inflection::util::StringViewUtils::join(dictionary.getPropertyNames(grammemes), u", ")
        + u"]";
}

const dictionary::DictionaryMetaData& DictionaryExposableMorphology::getDictionary() const {
    return dictionary;
}

const ::inflection::dictionary::Inflector& DictionaryExposableMorphology::getInflector() const{
    return inflector;
}

::inflection::util::ULocale DictionaryExposableMorphology::getLocale() const {
    return locale;
}

::std::vector<int64_t> DictionaryExposableMorphology::getWordGrammemeSets(::std::u16string_view word) const {
    int64_t wordGrammemes = 0;
    // Word not in dictionary
    if (dictionary.getCombinedBinaryType(&wordGrammemes, word) == nullptr) {
        return {};
    }
    ::std::vector<::inflection::dictionary::Inflector_InflectionPattern> inflectionPatterns;
    inflector.getInflectionPatternsForWord(word, inflectionPatterns);
    if (inflectionPatterns.empty()) {
        return {wordGrammemes};
    }

    ::std::vector<int64_t> wordGrammemesets;
    for (const auto& inflectionPattern : inflectionPatterns) {
        auto pos = inflectionPattern.getPartsOfSpeech();
        if (inflectionPattern.numInflections() == 0) {
            wordGrammemesets.push_back(pos);
            continue;
        }
        for (const auto& inflection : inflectionPattern.inflectionsForSurfaceForm(word, wordGrammemes)) {
            wordGrammemesets.push_back(inflection.getGrammemes() | pos);
        }
    }
    if (wordGrammemesets.empty()) {
        return {wordGrammemes};
    }
    return wordGrammemesets;
}

DictionaryExposableMorphology::DictionaryExposableMorphology(const ::inflection::util::ULocale &locale)
    : super()
    , dictionary(*npc(::inflection::dictionary::DictionaryMetaData::createDictionary(locale)))
    , inflector(::inflection::dictionary::Inflector::getInflector(locale))
    , locale(locale)
{
}

DictionaryExposableMorphology::~DictionaryExposableMorphology()
{
}

} // namespace inflection::analysis
