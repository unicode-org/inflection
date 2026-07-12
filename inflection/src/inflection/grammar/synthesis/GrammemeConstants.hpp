/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>

class inflection::grammar::synthesis::GrammemeConstants final
{
public:
    /** Part of speech */
    static constexpr auto POS = u"pos";
    static constexpr auto POS_ADJECTIVE = u"adjective";
    static constexpr auto POS_ADVERB = u"adverb";
    static constexpr auto POS_PARTICLE = u"particle";
    static constexpr auto POS_NOUN = u"noun";
    static constexpr auto POS_NUMERAL = u"numeral";
    static constexpr auto POS_ADPOSITION = u"adposition";
    static constexpr auto POS_PROPER_NOUN = u"proper-noun";
    static constexpr auto POS_VERB = u"verb";
    static constexpr auto POS_ARTICLE = u"article";
    static constexpr auto POS_DETERMINER = u"determiner";
    static constexpr auto POS_PRONOUN = u"pronoun";

    static constexpr auto CASE = u"case";
    static constexpr auto CASE_NOMINATIVE = u"nominative";
    static constexpr auto CASE_ABLATIVE = u"ablative";
    static constexpr auto CASE_ACCUSATIVE = u"accusative";
    static constexpr auto CASE_ADESSIVE = u"adessive";
    static constexpr auto CASE_ALLATIVE = u"allative";
    static constexpr auto CASE_BENEFACTIVE = u"benefactive";
    static constexpr auto CASE_COMITATIVE = u"comitative";
    static constexpr auto CASE_DATIVE = u"dative";
    static constexpr auto CASE_DIRECT = u"direct";
    static constexpr auto CASE_ELATIVE = u"elative";
    static constexpr auto CASE_ESSIVE = u"essive";
    static constexpr auto CASE_GENITIVE = u"genitive";
    static constexpr auto CASE_ILLATIVE = u"illative";
    static constexpr auto CASE_INESSIVE = u"inessive";
    static constexpr auto CASE_INSTRUMENTAL = u"instrumental";
    static constexpr auto CASE_LOCATIVE = u"locative";
    static constexpr auto CASE_OBLIQUE = u"oblique";
    static constexpr auto CASE_PARTITIVE = u"partitive";
    static constexpr auto CASE_PREPOSITIONAL = u"prepositional";
    static constexpr auto CASE_SOCIATIVE = u"sociative";
    static constexpr auto CASE_TRANSLATIVE = u"translative";
    static constexpr auto CASE_VOCATIVE = u"vocative";

    /** Grammatical count noun and grammatical number are similar topics. We really mean grammatical number these days. */
    static constexpr auto NUMBER = u"number";
    static constexpr auto NUMBER_SINGULAR = u"singular";
    static constexpr auto NUMBER_DUAL = u"dual";
    static constexpr auto NUMBER_PLURAL = u"plural";

    static constexpr auto GENDER = u"gender";
    static constexpr auto GENDER_MASCULINE = u"masculine";
    static constexpr auto GENDER_FEMININE = u"feminine";
    static constexpr auto GENDER_NEUTER = u"neuter";
    static constexpr auto GENDER_COMMON = u"common";

    static constexpr auto DEFINITENESS = u"definiteness";
    static constexpr auto DEFINITENESS_INDEFINITE = u"indefinite";
    static constexpr auto DEFINITENESS_DEFINITE = u"definite";
    static constexpr auto DEFINITENESS_CONSTRUCT = u"construct";

    static constexpr auto PERSON = u"person";
    static constexpr auto PERSON_FIRST = u"first";
    static constexpr auto PERSON_SECOND = u"second";
    static constexpr auto PERSON_THIRD = u"third";

    static constexpr auto ANIMACY = u"animacy";
    static constexpr auto ANIMACY_ANIMATE = u"animate";
    static constexpr auto ANIMACY_INANIMATE = u"inanimate";
    static constexpr auto ANIMACY_HUMAN = u"human";
    static constexpr auto ANIMACY_NONHUMAN = u"nonhuman";

    static constexpr auto MOOD = u"mood";
    static constexpr auto MOOD_IMPERATIVE = u"imperative";
    static constexpr auto MOOD_INDICATIVE = u"indicative";
    static constexpr auto MOOD_SUBJUNCTIVE = u"subjunctive";

    static constexpr auto TENSE = u"tense";
    static constexpr auto TENSE_PAST = u"past";
    static constexpr auto TENSE_PRESENT = u"present";
    static constexpr auto TENSE_FUTURE = u"future";

    static constexpr auto CLUSIVITY = u"clusivity";
    static constexpr auto CLUSIVITY_INCLUSIVE = u"inclusive";
    static constexpr auto CLUSIVITY_EXCLUSIVE = u"exclusive";

    static constexpr auto COMPARISON = u"comparison";
    static constexpr auto COMPARISON_PLAIN = u"plain";
    static constexpr auto COMPARISON_POSITIVE = u"positive";
    static constexpr auto COMPARISON_COMPARATIVE = u"comparative";
    static constexpr auto COMPARISON_SUPERLATIVE = u"superlative";

    static constexpr auto SIZENESS = u"sizeness";
    static constexpr auto SIZENESS_UNSIZED = u"unsized";
    static constexpr auto SIZENESS_DIMINUTIVE = u"diminutive";
    static constexpr auto SIZENESS_DOUBLE_DIMINUTIVE = u"double-diminutive";

    static constexpr auto VERBTYPE_INFINITIVE = u"infinitive";
    static constexpr auto VERBTYPE_GERUND = u"gerund";
    static constexpr auto VERBTYPE_PARTICIPLE = u"participle";

    static constexpr auto ADJECTIVAL_ATTRIBUTIVE = u"attributive";
    static constexpr auto ADJECTIVAL_PREDICATIVE = u"predicative";

    static constexpr auto REGISTER_INFORMAL = u"informal";
    static constexpr auto DERIVATIONTYPE_PATRONYMIC = u"patronymic";

    static constexpr auto ASPECT_PERFECTIVE = u"perfective";

private:
    GrammemeConstants() = delete;
};
