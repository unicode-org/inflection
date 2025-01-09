/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>

class inflection::grammar::synthesis::DaGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dialog::SemanticFeature* genderFeature {  };
    const ::inflection::dialog::SemanticFeature* countFeature {  };
    ::inflection::dialog::SpeakableString singularCommonString;
    ::inflection::dialog::SpeakableString singularNeuterString;
    ::inflection::dialog::SpeakableString pluralString;
    ::inflection::dialog::DictionaryLookupFunction countLookupFunction;
    ::inflection::dialog::DictionaryLookupFunction genderLookupFunction;

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    DaGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString);
    DaGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::inflection::dialog::SpeakableString& singularCommonString, const ::inflection::dialog::SpeakableString& singularNeuterString, const ::inflection::dialog::SpeakableString& pluralString);
};
