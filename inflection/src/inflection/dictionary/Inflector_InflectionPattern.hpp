/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/dictionary/Inflector_Inflection.hpp>
#include <inflection/dictionary/Inflector_MMappedDictionary.hpp>
#include <string>
#include <vector>

class inflection::dictionary::Inflector_InflectionPattern final
{
private: /* package */
    const int32_t identifierID { 0 };
    const int32_t frequencyIdx { 0 };
    const int64_t partsOfSpeech { 0 };
    const int16_t lemmaSuffixesLen { 0 };
    const int16_t numOfInflections { 0 };
    const int32_t lemmaSuffixesOffset {  };
    const int32_t inflectionsArrayStart {  };
    const ::inflection::dictionary::Inflector_MMappedDictionary& inflectorDictionary;

private:
    Inflector_Inflection getInflectionAtPosition(int32_t idx) const;
    ::std::u16string reinflectImplementation(int64_t fromGrammemes, int64_t toConstraints, const std::vector<int64_t> &toOptionalConstraints, std::u16string_view surfaceForm) const;

public:
    ::std::vector<Inflector_Inflection> constrain(const ::std::vector<::std::u16string>& constraints) const;
    ::std::vector<Inflector_Inflection> constrain(const ::std::vector<::std::u16string>& constraints, bool isSuperset) const;
private:
    ::std::optional<Inflector_Inflection> getMatchingLemmaInflection(const ::std::vector<Inflector_Inflection>& inflections) const;
public:
    ::std::optional<Inflector_Inflection> selectLemmaInflection(int64_t fromGrammemes, const ::std::vector<int64_t> &lemmaAttributes) const;
    ::std::vector<::inflection::dictionary::Inflector_Inflection> inflectionsForSurfaceForm(::std::u16string_view surfaceForm, int64_t fromGrammemes) const;
    ::std::u16string reinflect(int64_t fromGrammemes, int64_t toConstraints, std::u16string_view surfaceForm) const;
    ::std::u16string reinflectWithOptionalConstraints(int64_t fromGrammemes, int64_t toConstraints, const std::vector<int64_t> &toOptionalConstraints, std::u16string_view surfaceForm) const;
    bool containsSuffix(std::u16string_view suffix) const;

    int32_t getFrequency() const;
    ::std::u16string getIdentifier() const;
    int32_t numInflections() const;

    bool containsPartsOfSpeech(const ::std::u16string& pos) const;
    int64_t firstContainingPartOfSpeech(const ::std::vector<int64_t> &partOfSpeech) const;
    int64_t getPartsOfSpeech() const;

    std::weak_ordering operator<=>(const Inflector_InflectionPattern& other) const;

private:
    Inflector_InflectionPattern() = delete;

private: /* package */
    explicit Inflector_InflectionPattern(int32_t identifierID, int32_t frequencyIdx, int64_t partsOfSpeech, int16_t lemmaSuffixesLen,
                                         int32_t lemmaSuffixesIds, int16_t numOfInflections, int32_t inflectionsArrayStart,
                                         const Inflector_MMappedDictionary& containingDictionary);

private:
    friend class Inflector;
    friend class Inflector_MMappedDictionary;
    friend class Inflector_Suffix;
    friend class Inflector_Inflection;
};
