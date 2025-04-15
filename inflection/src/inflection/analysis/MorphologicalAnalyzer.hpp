/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/analysis/DictionaryExposableMorphology.hpp>
#include <inflection/dictionary/Inflector_Inflection.hpp>

class inflection::analysis::MorphologicalAnalyzer
    : public ::inflection::analysis::DictionaryExposableMorphology
{
public:
    typedef ::inflection::analysis::DictionaryExposableMorphology super;

protected:
    const ::std::vector<int64_t> lemmaAttributes;
    const ::std::vector<::std::vector<int64_t>> grammemePriorityTables;
    const ::std::vector<int64_t> ignoreGrammemeSets;

public:
    virtual ::std::optional<::inflection::dictionary::Inflector_Inflection> selectLemmaInflection(const dictionary::Inflector_InflectionPattern &inflectionPattern, int64_t inflectionGrammemes, int64_t wordGrammemesMask) const;
    int8_t compareGrammemes(int64_t grammemes1, int64_t grammemes2) const;
    void filterInflectionGrammemes(::std::u16string_view word, int64_t wordGrammemes, const ::std::vector<::inflection::dictionary::Inflector_InflectionPattern>& inflectionPatterns, ::std::vector<InflectionGrammemes> &inflectionGrammemes) const;

    MorphologicalAnalyzer(const ::inflection::util::ULocale& locale, const ::std::vector<::std::u16string_view> &lemmaAttributes, const std::vector<::std::vector<std::u16string_view>> &grammemePriorityStringTables = {}, const ::std::vector<::std::vector<::std::u16string>> &ignoreGrammemeSets = {});

    static std::vector<int64_t> convertLemmaAttributes(const dictionary::DictionaryMetaData &dictionary, const std::vector<::std::u16string_view> &lemmaAttributes);
};
