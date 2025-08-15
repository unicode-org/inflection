/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::GrammemeConstants final
{
private:
    GrammemeConstants() = delete;

public:
    /** Part of speech */
    static constexpr auto POS = u"pos";
    static const ::std::u16string& POS_ADJECTIVE();
    static const ::std::u16string& POS_PARTICLE();
    static const ::std::u16string& POS_NOUN();
    static const ::std::u16string& POS_ADPOSITION();
    static const ::std::u16string& POS_PROPER_NOUN();
    static const ::std::u16string& POS_VERB();
    static const ::std::u16string& POS_ARTICLE();
    static const ::std::u16string& POS_PRONOUN();

    static constexpr auto CASE = u"case";
    static const ::std::u16string& CASE_NOMINATIVE();
    static const ::std::u16string& CASE_ABLATIVE();
    static const ::std::u16string& CASE_ACCUSATIVE();
    static const ::std::u16string& CASE_ADESSIVE();
    static const ::std::u16string& CASE_ALLATIVE();
    static const ::std::u16string& CASE_COMITATIVE();
    static const ::std::u16string& CASE_DATIVE();
    static const ::std::u16string& CASE_DIRECT();
    static const ::std::u16string& CASE_ELATIVE();
    static const ::std::u16string& CASE_ESSIVE();
    static const ::std::u16string& CASE_GENITIVE();
    static const ::std::u16string& CASE_ILLATIVE();
    static const ::std::u16string& CASE_INESSIVE();
    static const ::std::u16string& CASE_INSTRUMENTAL();
    static const ::std::u16string& CASE_LOCATIVE();
    static const ::std::u16string& CASE_OBLIQUE();
    static const ::std::u16string& CASE_PARTITIVE();
    static const ::std::u16string& CASE_PREPOSITIONAL();
    static const ::std::u16string& CASE_SOCIATIVE();
    static const ::std::u16string& CASE_TRANSLATIVE();
    static const ::std::u16string& CASE_VOCATIVE();

    /** Grammatical count noun and grammatical number are similar topics. We really mean grammatical number these days. */
    static constexpr auto NUMBER = u"number";
    static const ::std::u16string& NUMBER_SINGULAR();
    static const ::std::u16string& NUMBER_DUAL();
    static const ::std::u16string& NUMBER_PLURAL();

    static constexpr auto GENDER = u"gender";
    static const ::std::u16string& GENDER_MASCULINE();
    static const ::std::u16string& GENDER_FEMININE();
    static const ::std::u16string& GENDER_NEUTER();
    static const ::std::u16string& GENDER_COMMON();

    static constexpr auto DEFINITENESS = u"definiteness";
    static const ::std::u16string& DEFINITENESS_INDEFINITE();
    static const ::std::u16string& DEFINITENESS_DEFINITE();
    static const ::std::u16string& DEFINITENESS_CONSTRUCT();

    static constexpr auto PERSON = u"person";
    static const ::std::u16string& PERSON_FIRST();
    static const ::std::u16string& PERSON_SECOND();
    static const ::std::u16string& PERSON_THIRD();

    static constexpr auto ANIMACY = u"animacy";
    static const ::std::u16string& ANIMACY_ANIMATE();
    static const ::std::u16string& ANIMACY_INANIMATE();
    static const ::std::u16string& ANIMACY_HUMAN();

    static constexpr auto MOOD = u"mood";
    static const ::std::u16string& MOOD_IMPERATIVE();
    static const ::std::u16string& MOOD_INDICATIVE();

    static constexpr auto TENSE = u"tense";
    static const ::std::u16string& TENSE_PAST();
    static const ::std::u16string& TENSE_PRESENT();
    static const ::std::u16string& TENSE_FUTURE();

    static constexpr auto CLUSIVITY = u"clusivity";
    static const ::std::u16string& CLUSIVITY_INCLUSIVE();
    static const ::std::u16string& CLUSIVITY_EXCLUSIVE();

    static constexpr auto COMPARISON = u"comparison";
    static const ::std::u16string& COMPARISON_PLAIN();
    static const ::std::u16string& COMPARISON_COMPARATIVE();
    static const ::std::u16string& COMPARISON_SUPERLATIVE();

    static constexpr auto SIZENESS = u"sizeness";
    static const ::std::u16string& SIZENESS_UNSIZED();
    static const ::std::u16string& SIZENESS_DIMINUTIVE();
    static const ::std::u16string& SIZENESS_DOUBLE_DIMINUTIVE();

    static const ::std::u16string& VERBTYPE_INFINITIVE();
    static const ::std::u16string& REGISTER_INFORMAL();
    static const ::std::u16string& DERIVATIONTYPE_PATRONYMIC();
    static const ::std::u16string& VERBTYPE_PARTICIPLE();

    static const ::std::u16string& ASPECT_PERFECTIVE();

};
