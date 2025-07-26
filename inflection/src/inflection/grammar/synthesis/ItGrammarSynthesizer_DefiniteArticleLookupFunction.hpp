/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::ItGrammarSynthesizer_DefiniteArticleLookupFunction
    : public ItGrammarSynthesizer_ArticleLookupFunction
{
public:
    typedef ItGrammarSynthesizer_ArticleLookupFunction super;

public: /* package */
    ::std::u16string simplePreposition {  };
    ::std::u16string singularMasculine {  };
    ::std::u16string singularMasculineWithConst {  };
    ::std::u16string singularFeminine {  };
    ::std::u16string singularWithVowel {  };
    ::std::u16string pluralMasculine {  };
    ::std::u16string pluralMasculineWithConst {  };
    ::std::u16string pluralFeminine {  };
public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Number numberValue, ItGrammarSynthesizer::Gender genderValue) const override;

    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& simplePreposition, const ::std::u16string& singularMasculine, const ::std::u16string& singularMasculineWithConst, const ::std::u16string& singularFeminine, const ::std::u16string& singularWithVowel, const ::std::u16string& pluralMasculine, const ::std::u16string& pluralMasculineWithConst, const ::std::u16string& pluralFeminine);
    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ItGrammarSynthesizer_DefiniteArticleLookupFunction& other);
    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ItGrammarSynthesizer_DefiniteArticleLookupFunction&) = delete;
    ItGrammarSynthesizer_DefiniteArticleLookupFunction& operator=(const ItGrammarSynthesizer_DefiniteArticleLookupFunction&) = delete;
};
