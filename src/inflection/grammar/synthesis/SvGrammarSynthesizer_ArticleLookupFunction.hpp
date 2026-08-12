/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>

class inflection::grammar::synthesis::SvGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    ::std::u16string singularCommonString {  };
    ::std::u16string singularNeuterString {  };
    ::std::u16string pluralString {  };
    ::inflection::dialog::DictionaryLookupFunction countLookupFunction;
    ::inflection::grammar::synthesis::SvGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    SvGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString);
    SvGrammarSynthesizer_ArticleLookupFunction(const SvGrammarSynthesizer_ArticleLookupFunction&) = delete;
    SvGrammarSynthesizer_ArticleLookupFunction& operator=(const SvGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
