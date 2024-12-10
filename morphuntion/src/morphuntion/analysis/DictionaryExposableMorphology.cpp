/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 4/14/22.
//
#include <morphuntion/analysis/DictionaryExposableMorphology.hpp>
#include <morphuntion/util/Logger.hpp>

namespace morphuntion::analysis {

DictionaryExposableMorphology::InflectionGrammemes::InflectionGrammemes(int64_t grammemes, std::optional<::morphuntion::dictionary::Inflector_Inflection> inflection)
: grammemes(grammemes), inflection(std::move(inflection))
{
}

std::u16string DictionaryExposableMorphology::InflectionGrammemes::getDescription(const ::morphuntion::dictionary::DictionaryMetaData &dictionary) const {
    return std::u16string(u"Inflection[")
        + (inflection.has_value()
            ? std::u16string(u"pattern-id=").append(inflection->getInflectionPattern()->getIdentifier())
                .append(u", suffix=\"").append(inflection->getSuffix()).append(u"\"")
            : std::u16string())
        + u"], grammemes["
        + morphuntion::util::StringViewUtils::join(dictionary.getPropertyNames(grammemes), u",")
        + u"]";
}

static int64_t inflectionGrammemesWithPartsOfSpeech(const morphuntion::dictionary::Inflector_Inflection &inflection) {
    return (inflection.getGrammemes() | inflection.getInflectionPattern()->getPartsOfSpeech());
}

void DictionaryExposableMorphology::getInflectionGrammemes(::std::u16string_view word, int64_t wordGrammemes, const ::std::vector<::morphuntion::dictionary::Inflector_InflectionPattern> &inflectionPatterns, ::std::vector<InflectionGrammemes> &inflectionGrammemes) const {
    for (const auto& inflectionPattern : inflectionPatterns){
        if (inflectionPattern.numInflections() == 0) {
            inflectionGrammemes.emplace_back(inflectionPattern.getPartsOfSpeech(), std::nullopt);
            continue;
        }
        const auto& inflections = inflectionPattern.inflectionsForSurfaceForm(word, wordGrammemes);
        for (const auto& inflection : inflections) {
            inflectionGrammemes.emplace_back(inflectionGrammemesWithPartsOfSpeech(inflection), inflection);
        }
    }
}

const dictionary::DictionaryMetaData& DictionaryExposableMorphology::getDictionary() const {
    return dictionary;
}

const ::morphuntion::dictionary::Inflector& DictionaryExposableMorphology::getInflector() const{
    return inflector;
}

::morphuntion::util::ULocale DictionaryExposableMorphology::getLocale() const {
    return locale;
}

::std::vector<int64_t> DictionaryExposableMorphology::getwordGrammemesets(::std::u16string_view word) const {
    int64_t wordGrammemes = 0;
    // Word not in dictionary
    if (dictionary.getCombinedBinaryType(&wordGrammemes, word) == nullptr) {
        return {};
    }
    ::std::vector<::morphuntion::dictionary::Inflector_InflectionPattern> inflectionPatterns;
    inflector.getInflectionPatternsForWord(word, inflectionPatterns);
    if (inflectionPatterns.empty()) {
        return {wordGrammemes};
    }
    ::std::vector<InflectionGrammemes> inflectionGrammemes;
    getInflectionGrammemes(word, wordGrammemes, inflectionPatterns, inflectionGrammemes);
    ::std::vector<int64_t> wordGrammemesets;
    for (const auto &[grammemes, inflection] : inflectionGrammemes) {
        wordGrammemesets.push_back(grammemes);
    }
    if (wordGrammemesets.empty()) {
        return {wordGrammemes};
    }
    return wordGrammemesets;
}

DictionaryExposableMorphology::DictionaryExposableMorphology(const ::morphuntion::util::ULocale &locale)
    : super()
    , dictionary(*npc(::morphuntion::dictionary::DictionaryMetaData::createDictionary(locale)))
    , inflector(::morphuntion::dictionary::Inflector::getInflector(locale))
    , locale(locale)
{

}

DictionaryExposableMorphology::~DictionaryExposableMorphology() {

}

} // namespace morphuntion::analysis
