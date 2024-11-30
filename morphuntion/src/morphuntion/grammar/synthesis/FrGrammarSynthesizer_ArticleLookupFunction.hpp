/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_CountLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_GenderLookupFunction.hpp>

class morphuntion::grammar::synthesis::FrGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::std::unique_ptr<::morphuntion::tokenizer::Tokenizer> tokenizer;
    FrGrammarSynthesizer_CountLookupFunction countLookupFunction {  };
    FrGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };
    const ::morphuntion::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    ::std::u16string defaultString {  };
    ::std::u16string defaultStartsWithVowelString {  };
    ::std::u16string singularMasculineString {  };
    ::std::u16string singularFeminineString {  };
    ::std::u16string singularStartsWithVowelString {  };
    ::std::u16string pluralString {  };
    bool applyContraction {  };
    bool applyVowelElisionToBothGenders {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    FrGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool insertSpace, bool applyContraction, bool applyVowelElisionToBothGenders, const char16_t* derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& defaultStartsWithVowelString, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& singularStartsWithVowelString, const ::std::u16string& pluralString);
    FrGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const FrGrammarSynthesizer_ArticleLookupFunction& other);
    ~FrGrammarSynthesizer_ArticleLookupFunction() override;
};
