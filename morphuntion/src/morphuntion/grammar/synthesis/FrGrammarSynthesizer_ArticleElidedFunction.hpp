/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>

class morphuntion::grammar::synthesis::FrGrammarSynthesizer_ArticleElidedFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    ::std::u16string defaultString {  };
    ::std::u16string dontDoElison {  };
    ::std::u16string doElision {  };
public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    FrGrammarSynthesizer_ArticleElidedFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& startsWithConsonant, const ::std::u16string& startsWithVowel);
};
