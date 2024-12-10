/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <cstdint>
#include <string>

class morphuntion::grammar::synthesis::ArGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel);

    enum class Count {
        undefined,
        singular,
        dual,
        plural
    };
    enum class Gender {
        undefined,
        masculine,
        feminine
    };
    enum class PronounNumber {
        undefined,
        singular,
        dual,
        plural
    };
    static PronounNumber getPronounNumber(const ::std::u16string* value);

    enum class PronounGender {
        undefined,
        masculine,
        feminine
    };
    static PronounGender getPronounGender(const ::std::u16string* value);

    enum class Person {
        undefined,
        first,
        second,
        third
    };
    static Person getPerson(const ::std::u16string* value);

    union LookupKey {
        struct {
            uint8_t field0 : 4;
            uint8_t field1 : 4;
            uint8_t field2 : 4;
        } fields;
        uint32_t value;
    };
    static LookupKey makeLookupKey(PronounNumber field0, PronounGender field1, Person field2);
    static LookupKey updateLookupKeyAttribute(LookupKey lookupKey, const ::std::u16string& attribute, const ::std::u16string* value);

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
