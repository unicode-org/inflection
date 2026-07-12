/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <cstdint>
#include <string>
#include <vector>

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
    static constexpr LookupKey makeLookupKey(Number num, Case kase) {
        return (static_cast<LookupKey>(kase) & 0xFF)
             | ((static_cast<LookupKey>(num)  & 0xFF) << 8);
    }
    static constexpr LookupKey makeVerbLookupKey(Person person, Number num, Tense tense, Mood mood) {
        return (static_cast<LookupKey>(person) & 0xFF)
             | ((static_cast<LookupKey>(num)    & 0xFF) << 8)
             | ((static_cast<LookupKey>(tense)  & 0x0F) << 24)
             | ((static_cast<LookupKey>(mood)   & 0x0F) << 28);
    }

    static LookupKey buildVerbSuffixKey(const std::vector<::std::u16string>& constraintValues);

    static ::std::u16string_view getSuffix(LookupKey key);
    static ::std::u16string_view getVerbSuffix(LookupKey key);

private:
    MlGrammarSynthesizer() = delete;
};
