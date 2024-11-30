/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_ArticleLookupFunction.hpp>
#include <string>

class morphuntion::grammar::synthesis::ItGrammarSynthesizer_DefiniteArticleLookupFunction
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
    ::morphuntion::dialog::SpeakableString* getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Count countValue, ItGrammarSynthesizer::Gender genderValue) const override;

    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& simplePreposition, const ::std::u16string& singularMasculine, const ::std::u16string& singularMasculineWithConst, const ::std::u16string& singularFeminine, const ::std::u16string& singularWithVowel, const ::std::u16string& pluralMasculine, const ::std::u16string& pluralMasculineWithConst, const ::std::u16string& pluralFeminine);
    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ItGrammarSynthesizer_DefiniteArticleLookupFunction& other);
};
