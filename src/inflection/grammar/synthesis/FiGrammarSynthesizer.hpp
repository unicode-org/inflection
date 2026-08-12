/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>
#include <vector>

class inflection::grammar::synthesis::FiGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);
    static ::std::vector<::std::vector<::std::vector<::std::u16string>>> getPossibleCombinations(const ::std::vector<::std::u16string>& arr);
    static bool hasBackVowels(const ::std::u16string& lemma);

    enum class Number {
        undefined,
        singular,
        plural
    };
    static Number getCount(const ::std::u16string* value);
    static ::std::u16string toString(FiGrammarSynthesizer::Number value);

    enum class Case {
        undefined,
        nominative,
        genitive,
        partitive,
        inessive,
        elative,
        illative,
        adessive,
        ablative,
        allative,
        essive,
        translative
    };
    static Case getCase(const ::std::u16string* value);
    static ::std::u16string toString(FiGrammarSynthesizer::Case value);

    enum class Person {
        undefined,
        first,
        second,
        third
    };
    static Person getPronounPerson(const ::std::u16string* value);

private:
    FiGrammarSynthesizer() = delete;

public:
    static constexpr auto RESIDING = u"residing";
    static constexpr auto EXITING = u"exiting";
    static constexpr auto ENTERING = u"entering";
    static constexpr auto WITH_HYPHEN_SUFFIX = u"withHyphenSuffix";
    static constexpr auto OUTER = u"outer";
};
