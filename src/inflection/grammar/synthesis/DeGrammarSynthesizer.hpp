/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <cstdint>
#include <string>

class inflection::grammar::synthesis::DeGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& model);

    enum class Number {
        undefined,
        plural,
        singular,
    };
    static Number getNumber(const ::std::u16string* value);

    enum class Gender {
        undefined,
        feminine,
        masculine,
        neuter,
    };
    static Gender getGender(const ::std::u16string* value);

    enum class Case {
        undefined,
        accusative,
        dative,
        genitive,
        nominative,
    };
    static Case getCase(const ::std::u16string* value);

    typedef uint32_t LookupKey;
    static constexpr LookupKey makeLookupKey(Number field0, Gender field1, Case field2) {
        return (static_cast<LookupKey>(field0) << 8)
            | (static_cast<LookupKey>(field1) << 4)
            | static_cast<LookupKey>(field2);
    }

private:
    DeGrammarSynthesizer() = default;

public:
    static constexpr auto PRONOUN = u"pronoun";
    static constexpr auto POSSESSIVE_PRONOUN = u"possessivePronoun";
    static constexpr auto DEF_ARTICLE = u"defArticle";
    static constexpr auto DEF_ARTICLE_IN_PREPOSITION = u"defArticleInPreposition";
    static constexpr auto NEG_ARTICLE = u"negArticle";
    static constexpr auto POSS_ARTICLE = u"possArticle";
    static constexpr auto INDEF_ARTICLE = u"indefArticle";
    static constexpr auto DEMON_ARTICLE = u"demonArticle";
    static constexpr auto INTERROGATIVE_ARTICLE = u"interrogativeArticle";
    static constexpr auto WITH_PRONOUN = u"withPronoun";
    static constexpr auto WITH_POSSESSIVE_PRONOUN = u"withPossessivePronoun";
    static constexpr auto WITH_DEF_ARTICLE_IN_PREPOSITION = u"withDefArticleInPreposition";
    static constexpr auto WITH_NEG_ARTICLE = u"withNegArticle";
    static constexpr auto WITH_POSS_ARTICLE = u"withPossArticle";
    static constexpr auto WITH_DEMON_ARTICLE = u"withDemonArticle";
    static constexpr auto WITH_INTERROGATIVE_ARTICLE = u"withInterrogativeArticle";
    static constexpr auto DECLENSION = u"declension";
    static constexpr auto DECLENSION_STRONG = u"strong";
    static constexpr auto DECLENSION_MIXED = u"mixed";
    static constexpr auto DECLENSION_WEAK = u"weak";
};
