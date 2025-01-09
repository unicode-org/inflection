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
    : DictionaryExposableMorphology(locale)
    , lemmaAttributes(convertLemmaAttributes(getDictionary(), lemmaAttributes))
    , grammemePriorityTables(convertGrammemePriorities(getDictionary(), grammemePriorityStringTables))
    , ignoreGrammemeSets(convertIgnoreGrammemes(getDictionary(), ignoreGrammemeSets))
{
}

int64_t MorphologicalAnalyzer::compareGrammemes(int64_t grammemes1, int64_t grammemes2) const {
    for (const auto &priorityValues : grammemePriorityTables) {
        const auto idx1 = ::std::find_if(priorityValues.begin(), priorityValues.end(), [&](const auto &priorityValue) { return (grammemes1 & priorityValue) == priorityValue;});
        const auto idx2 = ::std::find_if(priorityValues.begin(), priorityValues.end(), [&](const auto &priorityValue) { return (grammemes2 & priorityValue) == priorityValue;});
        if (idx1 != idx2) {
            return (idx1 < idx2) ? -1 : 1;
        }
    }
    return 0;
}

void MorphologicalAnalyzer::filterInflectionGrammemes(::std::vector<DictionaryExposableMorphology::InflectionGrammemes> &inflectionGrammemes) const{
    ::std::vector<DictionaryExposableMorphology::InflectionGrammemes> filteredInflectionGrammemes;
    for (const auto &item : inflectionGrammemes) {
        const auto grammemes = item.grammemes;
        const auto containsGrammemeSet = [grammemes](const int64_t grammemeSet){ return (grammemes & grammemeSet) == grammemeSet;};
        if (::std::find_if(ignoreGrammemeSets.begin(), ignoreGrammemeSets.end(), containsGrammemeSet) == ignoreGrammemeSets.end()) {
            filteredInflectionGrammemes.emplace_back(item);
        }
    }
    inflectionGrammemes.clear();
    for (const auto &inflection : filteredInflectionGrammemes) {
        inflectionGrammemes.emplace_back(inflection);
    }
}

} //namespace inflection::analysis
