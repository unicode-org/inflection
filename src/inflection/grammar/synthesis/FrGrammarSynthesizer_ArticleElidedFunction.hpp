/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>

class inflection::grammar::synthesis::FrGrammarSynthesizer_ArticleElidedFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    ::std::u16string defaultString {  };
    ::std::u16string dontDoElison {  };
    ::std::u16string doElision {  };
public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    FrGrammarSynthesizer_ArticleElidedFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& startsWithConsonant, const ::std::u16string& startsWithVowel);
    FrGrammarSynthesizer_ArticleElidedFunction(const FrGrammarSynthesizer_ArticleElidedFunction&) = delete;
    FrGrammarSynthesizer_ArticleElidedFunction& operator=(const FrGrammarSynthesizer_ArticleElidedFunction&) = delete;
};
