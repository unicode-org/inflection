/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/analysis/MorphologicalAnalyzer.hpp>
#include <algorithm>

using ::inflection::dictionary::Inflector_Inflection;

namespace inflection::analysis {

::std::vector<int64_t> MorphologicalAnalyzer::convertLemmaAttributes(const ::inflection::dictionary::DictionaryMetaData& dictionary, const ::std::vector<::std::u16string_view> &lemmaAttributes)
{
    ::std::vector<int64_t> result;
    result.reserve(lemmaAttributes.size());
    for (const auto &lemmaAttribute : lemmaAttributes) {
        result.push_back(dictionary.getBinaryProperties({::std::u16string(lemmaAttribute)}));
    }
    return result;
}

static ::std::vector<::std::vector<int64_t>> convertGrammemePriorities(const ::inflection::dictionary::DictionaryMetaData& dictionary, const std::vector<::std::vector<std::u16string_view>> &grammemePriorityStringTables)
{
    ::std::vector<::std::vector<int64_t>> grammemePriorityTables;
    grammemePriorityTables.reserve(grammemePriorityStringTables.size());
    for (const auto &grammemePriorityTableStrings : grammemePriorityStringTables) {
        ::std::vector<int64_t> grammemePriorityTable;
        grammemePriorityTable.reserve(grammemePriorityTableStrings.size());
        for (const auto &grammemeString : grammemePriorityTableStrings) {
            grammemePriorityTable.emplace_back(dictionary.getBinaryProperties({::std::u16string(grammemeString)}));
        }
        grammemePriorityTables.emplace_back(grammemePriorityTable);
    }
    return grammemePriorityTables;
}

static ::std::vector<int64_t> convertIgnoreGrammemes(const ::inflection::dictionary::DictionaryMetaData& dictionary, const ::std::vector<::std::vector<::std::u16string>> &ignoreGrammemeSets)
{
    ::std::vector<int64_t> result;
    result.reserve(ignoreGrammemeSets.size());
    for (const auto& grammemeSet : ignoreGrammemeSets) {
        result.emplace_back(dictionary.getBinaryProperties(grammemeSet));
    }
    return result;
}

MorphologicalAnalyzer::MorphologicalAnalyzer(const inflection::util::ULocale &locale, const ::std::vector<::std::u16string_view> &lemmaAttributes, const std::vector<::std::vector<std::u16string_view>> &grammemePriorityStringTables, const ::std::vector<::std::vector<::std::u16string>> &ignoreGrammemeSets)
    : super(locale)
    , lemmaAttributes(convertLemmaAttributes(getDictionary(), lemmaAttributes))
    , grammemePriorityTables(convertGrammemePriorities(getDictionary(), grammemePriorityStringTables))
    , ignoreGrammemeSets(convertIgnoreGrammemes(getDictionary(), ignoreGrammemeSets))
{
}

::std::optional<::inflection::dictionary::Inflector_Inflection> MorphologicalAnalyzer::selectLemmaInflection(const dictionary::Inflector_InflectionPattern &inflectionPattern, int64_t inflectionGrammemes, int64_t /*wordGrammemes*/) const {
    return inflectionPattern.selectLemmaInflection(inflectionGrammemes, lemmaAttributes);
}

int8_t MorphologicalAnalyzer::compareGrammemes(int64_t grammemes1, int64_t grammemes2) const {
    for (const auto &priorityValues : grammemePriorityTables) {
        const auto idx1 = ::std::find_if(priorityValues.begin(), priorityValues.end(), [&](const auto &priorityValue) { return (grammemes1 & priorityValue) == priorityValue;});
        const auto idx2 = ::std::find_if(priorityValues.begin(), priorityValues.end(), [&](const auto &priorityValue) { return (grammemes2 & priorityValue) == priorityValue;});
        if (idx1 != idx2) {
            return (idx1 < idx2) ? -1 : 1;
        }
    }
    return 0;
}

static bool isImportant(const int64_t grammemes, const ::std::vector<int64_t>& ignoreGrammemeSets) {
    const auto containsGrammemeSet = [grammemes](const int64_t grammemeSet){ return (grammemes & grammemeSet) == grammemeSet;};
    return std::ranges::none_of(ignoreGrammemeSets, containsGrammemeSet);
}

void MorphologicalAnalyzer::filterInflectionGrammemes(::std::u16string_view word,
                                                      int64_t wordGrammemes,
                                                      const ::std::vector<::inflection::dictionary::Inflector_InflectionPattern>& inflectionPatterns,
                                                      ::std::vector<InflectionGrammemes> &inflectionGrammemes) const
{
    for (const auto& inflectionPattern : inflectionPatterns) {
        auto pos = inflectionPattern.getPartsOfSpeech();
        if (inflectionPattern.numInflections() == 0) {
            if (isImportant(pos, ignoreGrammemeSets)) {
                inflectionGrammemes.emplace_back(pos, std::nullopt);
            }
            continue;
        }
        for (const auto& inflection : inflectionPattern.inflectionsForSurfaceForm(word, wordGrammemes)) {
            auto currGrammemes =  inflection.getGrammemes() | pos;
            if (isImportant(currGrammemes, ignoreGrammemeSets)) {
                inflectionGrammemes.emplace_back(currGrammemes, inflection);
            }
        }
    }
}

} //namespace inflection::analysis
