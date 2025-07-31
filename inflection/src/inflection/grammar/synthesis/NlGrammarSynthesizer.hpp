/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <string>

class inflection::grammar::synthesis::NlGrammarSynthesizer final
{
public:
    static void addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel);
public:
    static bool isVowelAtIndex(std::u16string_view str, int32_t index);
    static void guessDiminutive(::std::u16string* result, std::u16string_view guess);
public:

    enum class Number {
        undefined,
        singular,
        plural
    };
    static Number getNumber(const ::std::u16string& value);

    enum class Gender {
        undefined,
        masculine,
        feminine,
        neuter
    };
    static Gender getGender(const ::std::u16string& value);

    enum class Declension {
        undefined,
        declined,
        undeclined
    };
    static Declension getDeclension(const ::std::u16string& value);

    enum class Case {
        undefined,
        genitive,
        nominative
    };
    static Case getCase(const ::std::u16string& value);

private:
    NlGrammarSynthesizer() = delete;

public:
    static constexpr auto DECLENSION = u"declension";
    static const ::std::u16string& DECLENSION_UNDECLINED();
    static const ::std::u16string& DECLENSION_DECLINED();
    static constexpr auto DE = u"de";
    static constexpr auto DEZE = u"deze";
    static constexpr auto DIE = u"die";
    static constexpr auto ARTICLE_DEFINITE = u"defArticle";
    static constexpr auto ARTICLE_INDEFINITE = u"indefArticle";
};
