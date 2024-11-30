/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer_GenderLookupFunction.hpp>

class morphuntion::grammar::synthesis::SvGrammarSynthesizer_ArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* genderFeature {  };
    const ::morphuntion::dialog::SemanticFeature* countFeature {  };
    ::std::u16string singularCommonString {  };
    ::std::u16string singularNeuterString {  };
    ::std::u16string pluralString {  };
    ::morphuntion::dialog::DictionaryLookupFunction countLookupFunction;
    ::morphuntion::grammar::synthesis::SvGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    SvGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool includeSemanticValue, const ::std::u16string& singularCommonString, const ::std::u16string& singularNeuterString, const ::std::u16string& pluralString);
};
