/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/Inflector.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/analysis/MorphologicalAnalyzer.hpp>
#include <inflection/dialog/fwd.hpp>
#include <string>
#include <vector>
#include <list>

class inflection::dialog::DictionaryLookupInflector
    : public ::inflection::analysis::MorphologicalAnalyzer
{
public:
    typedef ::inflection::analysis::MorphologicalAnalyzer super;

private:
    bool enableDictionaryFallback;

private:
    int8_t compareInflectionGrammemes(const ::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes1, const ::inflection::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes2, const std::vector<int64_t> &disambiguationGrammemes) const;
    static int64_t disambiguationMatchScore(int64_t grammemes, const ::std::vector<int64_t> &disambiguationGrammemes);
    ::std::optional<::std::u16string> inflectWordImplementation(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const;
    ::std::optional<::std::u16string> inflectImplementation(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const;
public:
    ::std::optional<::std::u16string> inflect(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues = {}) const;
    
    /// Inflects the input word according to the supplied constraints and optional constraints
    /// - Parameters:
    ///   - word: The word to inflect.
    ///   - constraints: Input constraints, inflection returned should have all the constraints as part of its grammemes
    ///   - optionalConstraints: Input optional constraints, inflection returned doesn't need to have these constraints as part of its grammemes but inflections containing these constraints are preferred over those that don't
    ::std::optional<::std::u16string> inflectWithOptionalConstraints(std::u16string_view word, int64_t wordGrammemes, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues = {}) const;
    // Ideally this should be private but RuGrammarSynthesizer still uses it, don't use it new places
    ::std::optional<::std::u16string> inflectWord(std::u16string_view word, int64_t wordGrammemes, const ::std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues = {}) const;

public:
    explicit DictionaryLookupInflector(const ::inflection::util::ULocale &locale, const std::vector<::std::vector<std::u16string_view>> &propertyPrioritiesData = {}, const ::std::vector<::std::vector<::std::u16string>> &ignoreGrammemeSets = {}, bool enableDictionaryFallback = false);
    ~DictionaryLookupInflector() override;
};
