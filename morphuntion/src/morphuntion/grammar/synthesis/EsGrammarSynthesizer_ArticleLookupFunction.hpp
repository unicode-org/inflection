/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dictionary/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer_CountGenderLookupFunction.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <string>

class morphuntion::grammar::synthesis::EsGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{

public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const EsGrammarSynthesizer_CountGenderLookupFunction countLookupFunction;
    const EsGrammarSynthesizer_CountGenderLookupFunction genderLookupFunction;
    const morphuntion::dialog::DictionaryLookupFunction stressLookupFunction;
    const ::morphuntion::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::morphuntion::dialog::SemanticFeature &countFeature;
    const ::morphuntion::dialog::SemanticFeature &genderFeature;
    ::std::u16string_view defaultString {  };
    ::std::u16string_view singularMasculineString {  };
    ::std::u16string_view singularFeminineString {  };
    ::std::u16string_view pluralMasculineString {  };
    ::std::u16string_view pluralFeminineString {  };
    bool hasStressedSingularFeminineForm {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
    const char16_t* getDerivedSemanticName() const override;
    EsGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, ::std::u16string_view defaultString, ::std::u16string_view singularMasculineString, ::std::u16string_view singularFeminineString, ::std::u16string_view pluralMasculineString, ::std::u16string_view pluralFeminineString, bool hasStressedSingularFeminineForm);
    EsGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, ::std::u16string_view defaultString, ::std::u16string_view singularMasculineString, ::std::u16string_view singularFeminineString, ::std::u16string_view pluralMasculineString, ::std::u16string_view pluralFeminineString);
    EsGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName, const EsGrammarSynthesizer_ArticleLookupFunction& other);
};
