/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::PtGrammarSynthesizer final
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
        feminine
    };
    static Gender getGender(const ::std::u16string* value);

private:
    PtGrammarSynthesizer() = delete;

public:
    static constexpr auto ARTICLE_DEFINITE = u"defArticle";
    static constexpr auto ARTICLE_DE_PREPOSITION = u"dePrepArticle";
    static constexpr auto ARTICLE_EM_PREPOSITION = u"emPrepArticle";
    static constexpr auto ARTICLE_POR_PREPOSITION = u"porPrepArticle";
    static constexpr auto ARTICLE_INDEFINITE = u"indefArticle";
    static constexpr auto ARTICLE_INDEFINITE_EM_PREPOSITION = u"emPrepIndefArticle";
    static constexpr auto ARTICLE_A_PREPOSITION = u"aPrepArticle";
    static constexpr auto ARTICLE_POSSESSIVE = u"possArticle";
    static constexpr auto DEMONSTRATIVE_ADJECTIVE = u"demAdj";
    static constexpr auto DEMONSTRATIVE_IN_ADJECTIVE = u"inDemAdj";
    static constexpr auto DEMONSTRATIVE_OF_ADJECTIVE = u"ofDemAdj";
    static constexpr auto DEMONSTRATIVE_THIS_ADJECTIVE = u"thisDemAdj";
    static constexpr auto DEMONSTRATIVE_IN_THIS_ADJECTIVE = u"inThisDemAdj";
    static constexpr auto DEMONSTRATIVE_OF_THIS_ADJECTIVE = u"ofThisDemAdj";
    static constexpr auto DEMONSTRATIVE_THAT_ADJECTIVE = u"thatDemAdj";
    static constexpr auto DEMONSTRATIVE_IN_THAT_ADJECTIVE = u"inThatDemAdj";
    static constexpr auto DEMONSTRATIVE_OF_THAT_ADJECTIVE = u"ofThatDemAdj";
    static constexpr auto WITH_ARTICLE_DE_PREPOSITION = u"withDePrepArticle";
    static constexpr auto WITH_ARTICLE_EM_PREPOSITION = u"withEmPrepArticle";
    static constexpr auto WITH_ARTICLE_POR_PREPOSITION = u"withPorPrepArticle";
    static constexpr auto WITH_ARTICLE_INDEFINITE_EM_PREPOSITION = u"withEmPrepIndefArticle";
    static constexpr auto WITH_ARTICLE_A_PREPOSITION = u"withAPrepArticle";
    static constexpr auto WITH_ARTICLE_POSSESSIVE = u"withPossArticle";
    static constexpr auto WITH_DEMONSTRATIVE_ADJECTIVE = u"withDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_IN_ADJECTIVE = u"withInDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_OF_ADJECTIVE = u"withOfDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_THIS_ADJECTIVE = u"withThisDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_IN_THIS_ADJECTIVE = u"withInThisDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_OF_THIS_ADJECTIVE = u"withOfThisDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_THAT_ADJECTIVE = u"withThatDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_IN_THAT_ADJECTIVE = u"withInThatDemAdj";
    static constexpr auto WITH_DEMONSTRATIVE_OF_THAT_ADJECTIVE = u"withOfThatDemAdj";
};
