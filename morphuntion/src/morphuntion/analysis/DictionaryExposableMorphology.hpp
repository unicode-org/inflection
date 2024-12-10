/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 4/14/22.
//
#pragma once

#include <morphuntion/analysis/fwd.hpp>
#include <morphuntion/dictionary/DictionaryMetaData.hpp>
#include <morphuntion/dictionary/Inflector.hpp>

class morphuntion::analysis::DictionaryExposableMorphology
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;
    
    struct InflectionGrammemes {
        int64_t grammemes;
        std::optional<::morphuntion::dictionary::Inflector_Inflection> inflection;
        explicit InflectionGrammemes(int64_t grammemes, std::optional<::morphuntion::dictionary::Inflector_Inflection> inflection);
        std::u16string getDescription(const ::morphuntion::dictionary::DictionaryMetaData &dictionary) const;
    };

public:
    void getInflectionGrammemes(::std::u16string_view word, int64_t wordGrammemes, const ::std::vector<::morphuntion::dictionary::Inflector_InflectionPattern> &inflectionPatterns, ::std::vector<InflectionGrammemes> &inflectionGrammemes) const;
    ::std::vector<int64_t> getwordGrammemesets(::std::u16string_view word) const;
    const dictionary::DictionaryMetaData & getDictionary() const;
    const ::morphuntion::dictionary::Inflector& getInflector() const;
    ::morphuntion::util::ULocale getLocale() const;

public:
    explicit DictionaryExposableMorphology(const ::morphuntion::util::ULocale& locale);
    ~DictionaryExposableMorphology() override;

private:
    const ::morphuntion::dictionary::DictionaryMetaData &dictionary;
    const ::morphuntion::dictionary::Inflector &inflector;
    const ::morphuntion::util::ULocale locale;
};
