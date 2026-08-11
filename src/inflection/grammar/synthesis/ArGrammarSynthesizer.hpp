/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <cstdint>
#include <set>
#include <string>
#include <string_view>

class inflection::grammar::synthesis::ArGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);

    enum class Number {
        undefined,
        dual,
        plural,
        singular,
    };
    static Number getNumber(const ::std::u16string* value);

    enum class Gender {
        undefined,
        feminine,
        masculine,
    };
    static Gender getGender(const ::std::u16string* value);

    enum class Person {
        undefined,
        first,
        second,
        third,
    };
    static Person getPerson(const ::std::u16string* value);

    typedef uint32_t LookupKey;
    static constexpr LookupKey makeLookupKey(Number field0, Gender field1, Person field2) {
        return (static_cast<LookupKey>(field0) << 8)
            | (static_cast<LookupKey>(field1) << 4)
            | static_cast<LookupKey>(field2);
    }

    static const char16_t* getPossessivePronoun(LookupKey key);
    static std::set<std::u16string_view> getPossessivePronouns();

private:
    ArGrammarSynthesizer() = delete;

public:
    static constexpr auto PRONOUN_GENDER = u"genderPronoun";
    static constexpr auto PRONOUN_NUMBER = u"numberPronoun";
    static constexpr auto WITH_PRONOUN_POSSESSIVE = u"withPossessivePron";
    static constexpr auto PRONOUN_REFLEXIVE = u"reflexivePron";
    static constexpr auto WITH_PREPOSITION_TO = u"withToPrep";
    static constexpr auto WITH_PREPOSITION_WITH = u"withWithPrep";
    static constexpr auto WITH_PREPOSITION_AND = u"withAndPrep";
    static constexpr auto WITH_PREPOSITION_AS = u"withAsPrep";
};
