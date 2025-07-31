/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <cstdint>
#include <map>
#include <set>
#include <string>

class inflection::grammar::synthesis::ItGrammarSynthesizer final
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
        masculine,
        feminine,
    };
    static Gender getGender(const ::std::u16string* value);

public:
    static bool startsWithConsonantSubset(const ::std::u16string& word);
    static bool startsWithVowelForElision(const ::std::u16string& word);

private:
    ItGrammarSynthesizer() = delete;

public:
    static constexpr auto ARTICLE_DEFINITE = u"defArticle";
    static constexpr auto ARTICLE_INDEFINITE = u"indefArticle";
    static constexpr auto ARTICLE_A_PREPOSITION = u"aPrepArticle";
    static constexpr auto ARTICLE_DA_PREPOSITION = u"daPrepArticle";
    static constexpr auto ARTICLE_DE_PREPOSITION = u"dePrepArticle";
    static constexpr auto ARTICLE_IN_PREPOSITION = u"inPrepArticle";
    static constexpr auto ARTICLE_SU_PREPOSITION = u"suPrepArticle";
    static constexpr auto WITH_ARTICLE_A_PREPOSITION = u"withAPrepArticle";
    static constexpr auto WITH_ARTICLE_DA_PREPOSITION = u"withDaPrepArticle";
    static constexpr auto WITH_ARTICLE_DE_PREPOSITION = u"withDePrepArticle";
    static constexpr auto WITH_ARTICLE_IN_PREPOSITION = u"withInPrepArticle";
    static constexpr auto WITH_ARTICLE_SU_PREPOSITION = u"withSuPrepArticle";
};
