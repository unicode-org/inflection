/*
 * Copyright 2017-2026 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/tokenizer/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <memory>

class inflection::grammar::synthesis::FrGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::std::unique_ptr<::inflection::tokenizer::Tokenizer> tokenizer;
    const ::inflection::dialog::DictionaryLookupFunction& numberLookupFunction;
    const ::inflection::dialog::DictionaryLookupFunction& genderLookupFunction;
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::inflection::dialog::SemanticFeature* numberFeature {  };
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    ::std::u16string_view defaultString {  };
    ::std::u16string_view defaultStartsWithVowelString {  };
    ::std::u16string_view singularMasculineString {  };
    ::std::u16string_view singularFeminineString {  };
    ::std::u16string_view singularStartsWithVowelString {  };
    ::std::u16string_view pluralString {  };
    bool applyContraction {  };
    bool applyVowelElisionToBothGenders {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    FrGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model,
                                               const ::inflection::dialog::DictionaryLookupFunction& numberLookupFunction,
                                               const ::inflection::dialog::DictionaryLookupFunction& genderLookupFunction,
                                               bool insertSpace,
                                               bool applyContraction,
                                               bool applyVowelElisionToBothGenders,
                                               const char16_t* derivedSemanticName,
                                               std::u16string_view defaultString,
                                               std::u16string_view defaultStartsWithVowelString,
                                               std::u16string_view singularMasculineString,
                                               std::u16string_view singularFeminineString,
                                               std::u16string_view singularStartsWithVowelString,
                                               std::u16string_view pluralString);
    FrGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const FrGrammarSynthesizer_ArticleLookupFunction& other);
    ~FrGrammarSynthesizer_ArticleLookupFunction() override;
    FrGrammarSynthesizer_ArticleLookupFunction(const FrGrammarSynthesizer_ArticleLookupFunction&) = delete;
    FrGrammarSynthesizer_ArticleLookupFunction& operator=(const FrGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
