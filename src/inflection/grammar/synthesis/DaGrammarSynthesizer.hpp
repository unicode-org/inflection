/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::DaGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);

    enum class Number {
        undefined,
        singular,
        plural
    };
    static Number getNumber(const ::std::u16string* value);

    enum class Gender {
        undefined,
        common,
        neuter
    };
    static Gender getGender(const ::std::u16string* value);

private:
    DaGrammarSynthesizer() = delete;

public:
    static constexpr auto ARTICLE_NEGATIVE = u"negArticle";
    static constexpr auto WITH_ARTICLE_NEGATIVE = u"withNegArticle";
    static constexpr auto ARTICLE_DEFINITE = u"defArticle";
    static constexpr auto WITH_ARTICLE_DEFINITE = u"withDefArticle";
    static constexpr auto ARTICLE_INDEFINITE = u"indefArticle";
    static constexpr auto WITH_ARTICLE_INDEFINITE = u"withIndefArticle";
    static constexpr auto ARTICLE_POSSESSIVE = u"possArticle";
    static constexpr auto WITH_ARTICLE_POSSESSIVE = u"withPossArticle";
    static constexpr auto ARTICLE_NEW = u"newArticle";
    static constexpr auto WITH_ARTICLE_NEW = u"withNewArticle";
    static constexpr auto ARTICLE_OTHER = u"otherArticle";
    static constexpr auto WITH_ARTICLE_OTHER = u"withOtherArticle";
    static constexpr auto ARTICLE_INTERROGATIVE = u"interrogativeArticle";
    static constexpr auto WITH_ARTICLE_INTERROGATIVE = u"withInterrogativeArticle";
    static constexpr auto PRONOUN_INDEFINITE = u"indefPron";
    static constexpr auto WITH_PRONOUN_INDEFINITE = u"withIndefPron";
    static constexpr auto PRONOUN_DEMONSTRATIVE = u"demonArticle";
    static constexpr auto WITH_PRONOUN_DEMONSTRATIVE = u"withDemonArticle";
};
