/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::NbGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    ::std::u16string singularMasculineString {  };
    ::std::u16string singularFeminineString {  };
    ::std::u16string singularNeuterString {  };
    ::std::u16string pluralString {  };
    ::inflection::dialog::DictionaryLookupFunction countLookupFunction;
    ::inflection::dialog::DictionaryLookupFunction genderLookupFunction;
public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    NbGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularMasculineString, const ::std::u16string& singularFeminineString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString);
    NbGrammarSynthesizer_ArticleLookupFunction(const NbGrammarSynthesizer_ArticleLookupFunction&) = delete;
    NbGrammarSynthesizer_ArticleLookupFunction& operator=(const NbGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
