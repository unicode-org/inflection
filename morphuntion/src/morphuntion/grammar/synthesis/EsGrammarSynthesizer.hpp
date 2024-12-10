/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <string>

class morphuntion::grammar::synthesis::EsGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel);

    enum class Count {
        undefined,
        singular,
        plural
    };
    static Count getCount(const ::std::u16string* value);

    enum class Gender {
        undefined,
        masculine,
        feminine,
    };
    static Gender getGender(const ::std::u16string* value);

private:
    EsGrammarSynthesizer() = delete;

public:
    static constexpr auto ARTICLE_DEFINITE = u"defArticle";
    static constexpr auto ARTICLE_INDEFINITE = u"indefArticle";
    static constexpr auto ARTICLE_DE_PREPOSITION = u"dePrepArticle";
    static constexpr auto ARTICLE_A_PREPOSITION = u"aPrepArticle";
    static constexpr auto DEMONSTRATIVE_ADJECTIVE = u"demAdj";
    static constexpr auto WITH_ARTICLE_DE_PREPOSITION = u"withDePrepArticle";
    static constexpr auto WITH_ARTICLE_A_PREPOSITION = u"withAPrepArticle";
    static constexpr auto WITH_DEMONSTRATIVE_ADJECTIVE = u"withDemAdj";
};
