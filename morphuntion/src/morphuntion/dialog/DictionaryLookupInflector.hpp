/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dictionary/Inflector.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/analysis/MorphologicalAnalyzer.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <morphuntion/dialog/DefaultFeatureFunction.hpp>
#include <string>
#include <vector>
#include <list>

class morphuntion::dialog::DictionaryLookupInflector
    : public ::morphuntion::analysis::MorphologicalAnalyzer
{
public:
    typedef ::morphuntion::analysis::MorphologicalAnalyzer super;

private:
    bool enableDictionaryFallback;

private:
    int64_t compareInflectionGrammemes(const ::morphuntion::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes1, const ::morphuntion::analysis::DictionaryExposableMorphology::InflectionGrammemes &inflectionGrammemes2, const std::vector<int64_t> &disambiguationGrammemes) const;
    int64_t disambiguationMatchScore(int64_t grammemes, const ::std::vector<int64_t> &disambiguationGrammemes) const;
    ::std::optional<::std::u16string> inflectWordImplementation(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const;
    ::std::optional<::std::u16string> inflectImplementation(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues) const;
public:
    ::std::optional<::std::u16string> inflect(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues = {}) const;
    
    /// Inflects the input word according to the supplied constraints and optional constraints
    /// - Parameters:
    ///   - word: The word to inflect.
    ///   - constraints: Input constraints, inflection returned should have all the constraints as part of its grammemes
    ///   - optionalConstraints: Input optional constraints, inflection returned doesn't need to have these constraints as part of its grammemes but inflections containing these constraints are preferred over those that don't
    ::std::optional<::std::u16string> inflectWithOptionalConstraints(std::u16string_view word, const std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &optionalConstraints, const std::vector<::std::u16string> &disambiguationGrammemeValues = {}) const;
    // Ideally this should be private but RuGrammarSynthesizer still uses it, don't use it new places
    ::std::optional<::std::u16string> inflectWord(std::u16string_view word, const ::std::vector<::std::u16string> &constraints, const std::vector<::std::u16string> &disambiguationGrammemeValues = {}) const;

public:
    explicit DictionaryLookupInflector(const ::morphuntion::util::ULocale &locale, const std::vector<::std::vector<std::u16string_view>> &propertyPrioritiesData = {}, const ::std::vector<::std::vector<::std::u16string>> &ignoreGrammemeSets = {}, bool enableDictionaryFallback = false);
    ~DictionaryLookupInflector() override;
};
