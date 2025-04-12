/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_GenderLookupFunction.hpp>

class inflection::grammar::synthesis::FrGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    FrGrammarSynthesizer_CountLookupFunction countLookupFunction {  };
    FrGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    ::std::u16string defaultString {  };
    ::std::u16string defaultStartsWithVowelString {  };
    ::std::u16string singularMasculineString {  };
    ::std::u16string singularFeminineString {  };
    ::std::u16string singularStartsWithVowelString {  };
    ::std::u16string pluralString {  };
    bool applyContraction {  };
    bool applyVowelElisionToBothGenders {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    FrGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool insertSpace, bool applyContraction, bool applyVowelElisionToBothGenders, const char16_t* derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& defaultStartsWithVowelString, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& singularStartsWithVowelString, const ::std::u16string& pluralString);
    FrGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const FrGrammarSynthesizer_ArticleLookupFunction& other);
    ~FrGrammarSynthesizer_ArticleLookupFunction() override;
    FrGrammarSynthesizer_ArticleLookupFunction(const FrGrammarSynthesizer_ArticleLookupFunction&) = delete;
    FrGrammarSynthesizer_ArticleLookupFunction& operator=(const FrGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
