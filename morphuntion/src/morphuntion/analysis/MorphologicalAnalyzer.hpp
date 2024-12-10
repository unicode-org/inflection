/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/analysis/fwd.hpp>
#include <morphuntion/analysis/DictionaryExposableMorphology.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/dictionary/Inflector.hpp>

class morphuntion::analysis::MorphologicalAnalyzer
    : public ::morphuntion::analysis::DictionaryExposableMorphology
{

protected:
    const ::std::vector<int64_t> lemmaAttributes;
    const ::std::vector<::std::vector<int64_t>> grammemePriorityTables;
    const ::std::vector<int64_t> ignoreGrammemeSets;

public:
    virtual void filterInflectionGrammemes(::std::vector<DictionaryExposableMorphology::InflectionGrammemes> &inflectionGrammemes) const;
    virtual int64_t compareGrammemes(int64_t grammemes1, int64_t grammemes2) const;

    MorphologicalAnalyzer(const ::morphuntion::util::ULocale& locale, const ::std::vector<::std::u16string_view> &lemmaAttributes, const std::vector<::std::vector<std::u16string_view>> &grammemePriorityStringTables = {}, const ::std::vector<::std::vector<::std::u16string>> &ignoreGrammemeSets = {});

    static std::vector<int64_t> convertLemmaAttributes(const dictionary::DictionaryMetaData &dictionary, const std::vector<::std::u16string_view> &lemmaAttributes);
};
