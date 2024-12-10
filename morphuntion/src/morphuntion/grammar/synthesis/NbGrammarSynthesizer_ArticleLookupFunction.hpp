/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>

class morphuntion::grammar::synthesis::NbGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    ::std::u16string singularMasculineString {  };
    ::std::u16string singularFeminineString {  };
    ::std::u16string singularNeuterString {  };
    ::std::u16string pluralString {  };
    ::morphuntion::dialog::DictionaryLookupFunction countLookupFunction;
    ::morphuntion::dialog::DictionaryLookupFunction genderLookupFunction;
public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    NbGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString);
};
