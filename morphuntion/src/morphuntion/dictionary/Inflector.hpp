/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/Inflector_MMappedDictionary.hpp>
#include <morphuntion/dictionary/Inflector_InflectionPattern.hpp>
#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <map>
#include <optional>
#include <string>

class morphuntion::dictionary::Inflector
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

public:
    std::optional<morphuntion::dictionary::Inflector_InflectionPattern> getInflectionPatternByName(std::u16string_view name) const;
    std::vector<morphuntion::dictionary::Inflector_InflectionPattern> getInflectionPatternsBySuffix(std::u16string_view suffix) const;
    void getInflectionPatternsForWord(std::u16string_view word, ::std::vector<Inflector_InflectionPattern> &inflectionPatterns) const;

    /**
     * Factory method to return a Inflector singleton for each locale.
     * @param locale The locale to get the inflector.
     * @return A singleton. Do not delete this object.
     */
    static const Inflector& getInflector(const ::morphuntion::util::ULocale& locale);

private:
    explicit Inflector(morphuntion::util::MemoryMappedFile& memoryMappedFile, const ::std::u16string& sourcePath, const ::morphuntion::dictionary::DictionaryMetaData_MMappedDictionary &dictionary);

    ~Inflector() override;

private:
    Inflector(const Inflector& other) = delete;
    Inflector& operator=(const Inflector& other) = delete;
    const Inflector_MMappedDictionary mmappedDictionary;

    friend class DictionaryMetaData_MMappedDictionary;
    friend class Inflector_Inflection;
    friend class Inflector_InflectionPattern;
};
