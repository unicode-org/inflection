/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>

class morphuntion::grammar::synthesis::DaGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    ::morphuntion::dialog::SpeakableString singularCommonString;
    ::morphuntion::dialog::SpeakableString singularNeuterString;
    ::morphuntion::dialog::SpeakableString pluralString;
    ::morphuntion::dialog::DictionaryLookupFunction countLookupFunction;
    ::morphuntion::dialog::DictionaryLookupFunction genderLookupFunction;

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    DaGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString);
    DaGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::morphuntion::dialog::SpeakableString& singularCommonString, const ::morphuntion::dialog::SpeakableString& singularNeuterString, const ::morphuntion::dialog::SpeakableString& pluralString);
};
