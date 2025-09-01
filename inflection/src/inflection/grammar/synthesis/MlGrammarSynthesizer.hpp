/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <map>

class inflection::grammar::synthesis::MlGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& model);

    enum class Number {
        undefined,
        singular,
        plural
    };
    static Number getNumber(const ::std::u16string& value);

    enum class Case {
        undefined,
        nominative,
        accusative,
        dative,
        genitive,
        instrumental,
        locative
    };
    static Case getCase(const ::std::u16string& value);

    enum class Person {
        undefined,
        first,
        second,
        third
    };
    static Person getPerson(const ::std::u16string& value);

    enum class Tense {
        undefined,
        past,
        present,
        future
    };
    static Tense getTense(const ::std::u16string& value);

    enum class Mood {
        undefined,
        indicative,
        imperative,
        subjunctive
    };
    static Mood getMood(const ::std::u16string& value);

    typedef uint32_t LookupKey;
    static LookupKey makeLookupKey(Number num, Case kase);
    static LookupKey makeVerbLookupKey(Person person, Number num, Tense tense, Mood mood);

    static LookupKey buildVerbSuffixKey(const std::vector<::std::u16string>& constraintValues);

private:
    static const std::map<LookupKey, ::std::u16string_view>& MALAYALAM_SUFFIX_MAP();
    static const std::map<LookupKey, ::std::u16string_view>& MALAYALAM_VERB_SUFFIX_MAP();

public:
    static ::std::u16string_view getSuffix(LookupKey key);
    static ::std::u16string_view getVerbSuffix(LookupKey key);

private:
    MlGrammarSynthesizer() = delete;
};
