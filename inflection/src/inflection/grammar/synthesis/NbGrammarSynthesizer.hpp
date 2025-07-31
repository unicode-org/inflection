/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::NbGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);

    enum class Number {
        UNDEFINED,
        SINGULAR,
        PLURAL
    };
    static Number getNumber(const ::std::u16string* value);

    enum class Gender {
        UNDEFINED,
        MASCULINE,
        FEMININE,
        NEUTER
    };
    static Gender getGender(const ::std::u16string* value);

private:
    NbGrammarSynthesizer() = delete;

public:
    static constexpr auto DEF_ARTICLE = u"defArticle";
    static constexpr auto INDEF_ARTICLE = u"indefArticle";
    static constexpr auto INTERROGATIVE_ARTICLE = u"interrogativeArticle";
    static constexpr auto POSS_ARTICLE = u"possArticle";
    static constexpr auto DEMON_ARTICLE = u"demonArticle";
    static constexpr auto WITH_DEF_ARTICLE = u"withDefArticle";
    static constexpr auto WITH_INDEF_ARTICLE = u"withIndefArticle";
    static constexpr auto WITH_INTERROGATIVE_ARTICLE = u"withInterrogativeArticle";
    static constexpr auto WITH_POSS_ARTICLE = u"withPossArticle";
    static constexpr auto WITH_DEMON_ARTICLE = u"withDemonArticle";
};
