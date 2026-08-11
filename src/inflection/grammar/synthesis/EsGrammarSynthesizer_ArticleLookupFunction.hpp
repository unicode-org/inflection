/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer_CountGenderLookupFunction.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::EsGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{

public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const EsGrammarSynthesizer_CountGenderLookupFunction countLookupFunction;
    const EsGrammarSynthesizer_CountGenderLookupFunction genderLookupFunction;
    const inflection::dialog::DictionaryLookupFunction stressLookupFunction;
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::inflection::dialog::SemanticFeature &countFeature;
    const ::inflection::dialog::SemanticFeature &genderFeature;
    ::std::u16string_view defaultString {  };
    ::std::u16string_view singularMasculineString {  };
    ::std::u16string_view singularFeminineString {  };
    ::std::u16string_view pluralMasculineString {  };
    ::std::u16string_view pluralFeminineString {  };
    bool hasStressedSingularFeminineForm {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    const char16_t* getDerivedSemanticName() const override;
    EsGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, ::std::u16string_view defaultString, ::std::u16string_view singularMasculineString, ::std::u16string_view singularFeminineString, ::std::u16string_view pluralMasculineString, ::std::u16string_view pluralFeminineString, bool hasStressedSingularFeminineForm);
    EsGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, ::std::u16string_view defaultString, ::std::u16string_view singularMasculineString, ::std::u16string_view singularFeminineString, ::std::u16string_view pluralMasculineString, ::std::u16string_view pluralFeminineString);
    EsGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, const EsGrammarSynthesizer_ArticleLookupFunction& other);
    EsGrammarSynthesizer_ArticleLookupFunction(const EsGrammarSynthesizer_ArticleLookupFunction&) = delete;
    EsGrammarSynthesizer_ArticleLookupFunction& operator=(const EsGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
