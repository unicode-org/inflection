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
    static Number getNumber(const ::std::u16string* value);

    enum class Case {
        undefined,
        nominative,
        accusative,
        dative,
        genitive,
        instrumental,
        locative
    };
    static Case getCase(const ::std::u16string* value);

    enum class Person {
        undefined,
        first,
        second,
        third
    };
    static Person getPerson(const ::std::u16string* value);

    enum class Tense {
        undefined,
        past,
        present,
        future
    };
    static Tense getTense(const ::std::u16string* value);

    enum class Mood {
        undefined,
        indicative,
        imperative,
        subjunctive
    };
    static Mood getMood(const ::std::u16string* value);

    typedef uint32_t LookupKey;
    static LookupKey makeLookupKey(Number num, Case kase);
    static LookupKey makeVerbLookupKey(Person person, Number num, Tense tense, Mood mood);

    static Person personFromConstraint(const ::std::u16string& val);
    static Number numberFromConstraint(const ::std::u16string& val);
    static Case caseFromConstraint(const ::std::u16string& val);

    static LookupKey buildVerbSuffixKey(const std::vector<::std::u16string>& constraintValues);

    static const std::map<LookupKey, ::std::u16string> malayalamSuffixMap;
    static const std::map<LookupKey, ::std::u16string> malayalamVerbSuffixMap;

    static const ::std::u16string& getSuffix(LookupKey key);
    static const ::std::u16string& getVerbSuffix(LookupKey key);

private:
    MlGrammarSynthesizer() = delete;

public:
    static constexpr auto NOUN_CASE   = u"case";
    static constexpr auto NOUN_NUMBER = u"number";
    static constexpr auto VERB_PERSON = u"person";
    static constexpr auto VERB_NUMBER = u"verbNumber";
    static constexpr auto VERB_TENSE  = u"tense";
    static constexpr auto VERB_MOOD   = u"mood";
};
