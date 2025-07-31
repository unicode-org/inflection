/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::SvGrammarSynthesizer final
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
        neuter,
    };
    static Gender getGender(const ::std::u16string* value);

private:
    SvGrammarSynthesizer() = delete;

public:
    static constexpr auto DEF_ARTICLE = u"defArticle";
    static constexpr auto INDEF_ARTICLE = u"indefArticle";
    static constexpr auto INDEFINITE_PRONOUN = u"indefPron";
    static constexpr auto INTERROGATIVE_ARTICLE = u"interrogativeArticle";
    static constexpr auto NEG_ARTICLE = u"negArticle";
    static constexpr auto NEW_ARTICLE = u"newArticle";
    static constexpr auto OTHER_ARTICLE = u"otherArticle";
    static constexpr auto POSS_ARTICLE = u"possArticle";
    static constexpr auto WITH_INDEFINITE_PRONOUN = u"withIndefPron";
    static constexpr auto WITH_INTERROGATIVE_ARTICLE = u"withInterrogativeArticle";
    static constexpr auto WITH_NEG_ARTICLE = u"withNegArticle";
    static constexpr auto WITH_NEW_ARTICLE = u"withNewArticle";
    static constexpr auto WITH_OTHER_ARTICLE = u"withOtherArticle";
    static constexpr auto WITH_POSS_ARTICLE = u"withPossArticle";
};
