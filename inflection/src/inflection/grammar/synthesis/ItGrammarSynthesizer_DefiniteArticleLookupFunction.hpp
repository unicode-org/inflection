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
    ::std::u16string_view simplePreposition {  };
    ::std::u16string_view singularMasculine {  };
    ::std::u16string_view singularMasculineWithConst {  };
    ::std::u16string_view singularFeminine {  };
    ::std::u16string_view singularWithVowel {  };
    ::std::u16string_view pluralMasculine {  };
    ::std::u16string_view pluralMasculineWithConst {  };
    ::std::u16string_view pluralFeminine {  };
public: /* package */
    ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Number numberValue, ItGrammarSynthesizer::Gender genderValue) const override;

    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model,
                                                       const ::inflection::dialog::DictionaryLookupFunction& numberLookupFunction,
                                                       const ::inflection::dialog::DictionaryLookupFunction& genderLookupFunction,
                                                       const char16_t* derivedSemanticName,
                                                       std::u16string_view simplePreposition,
                                                       std::u16string_view singularMasculine,
                                                       std::u16string_view singularMasculineWithConst,
                                                       std::u16string_view singularFeminine,
                                                       std::u16string_view singularWithVowel,
                                                       std::u16string_view pluralMasculine,
                                                       std::u16string_view pluralMasculineWithConst,
                                                       std::u16string_view pluralFeminine);
    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, const ItGrammarSynthesizer_DefiniteArticleLookupFunction& other);
    ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ItGrammarSynthesizer_DefiniteArticleLookupFunction&) = delete;
    ItGrammarSynthesizer_DefiniteArticleLookupFunction& operator=(const ItGrammarSynthesizer_DefiniteArticleLookupFunction&) = delete;
};
