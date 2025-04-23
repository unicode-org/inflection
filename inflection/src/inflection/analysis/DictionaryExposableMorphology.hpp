/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 4/14/22.
//
#pragma once

#include <inflection/analysis/fwd.hpp>
#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/dictionary/Inflector.hpp>

class inflection::analysis::DictionaryExposableMorphology
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;
    
    struct InflectionGrammemes {
        int64_t grammemes;
        std::optional<::inflection::dictionary::Inflector_Inflection> inflection;
        explicit InflectionGrammemes(int64_t grammemes, std::optional<::inflection::dictionary::Inflector_Inflection> inflection);
        std::u16string getDescription(const ::inflection::dictionary::DictionaryMetaData &dictionary) const;
    };

public:
    ::std::vector<int64_t> getWordGrammemeSets(::std::u16string_view word) const;
    const dictionary::DictionaryMetaData & getDictionary() const;
    const ::inflection::dictionary::Inflector& getInflector() const;
    ::inflection::util::ULocale getLocale() const;

public:
    explicit DictionaryExposableMorphology(const ::inflection::util::ULocale& locale);
    ~DictionaryExposableMorphology() override;

private:
    const ::inflection::dictionary::DictionaryMetaData &dictionary;
    const ::inflection::dictionary::Inflector &inflector;
    const ::inflection::util::ULocale locale;
};
