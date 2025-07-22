/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/Inflector_MMappedDictionary.hpp>
#include <inflection/dictionary/Inflector_InflectionPattern.hpp>
#include <inflection/dictionary/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/Object.hpp>
#include <map>
#include <optional>
#include <string>

class inflection::dictionary::Inflector
    : public virtual ::inflection::Object
{
public:
    typedef ::inflection::Object super;

public:
    std::optional<inflection::dictionary::Inflector_InflectionPattern> getInflectionPatternByName(std::u16string_view name) const;
    std::vector<inflection::dictionary::Inflector_InflectionPattern> getInflectionPatternsBySuffix(std::u16string_view suffix) const;
    void getInflectionPatternsForWord(std::u16string_view word, ::std::vector<Inflector_InflectionPattern> &inflectionPatterns) const;

    /**
     * Factory method to return a Inflector singleton for each locale.
     * @param locale The locale to get the inflector.
     * @return A singleton. Do not delete this object.
     */
    static const Inflector& getInflector(const ::inflection::util::ULocale& locale);

private:
    explicit Inflector(inflection::util::MemoryMappedFile& memoryMappedFile, const ::std::u16string& sourcePath, const ::inflection::dictionary::DictionaryMetaData_MMappedDictionary &dictionary);

public:
    ~Inflector() override;

private:
    Inflector(const Inflector& other) = delete;
    Inflector& operator=(const Inflector& other) = delete;
    const Inflector_MMappedDictionary mmappedDictionary;

    friend class DictionaryMetaData_MMappedDictionary;
    friend class Inflector_Inflection;
    friend class Inflector_InflectionPattern;
};
