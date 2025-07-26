/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_GenderLookupFunction.hpp>

class inflection::grammar::synthesis::ItGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    const ::inflection::dialog::SemanticFeature& countFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    ItGrammarSynthesizer_CountLookupFunction countLookupFunction {  };
    ItGrammarSynthesizer_GenderLookupFunction genderLookupFunction {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

public: /* package */
    virtual ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Number numberValue, ItGrammarSynthesizer::Gender genderValue) const = 0;

    ItGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    ItGrammarSynthesizer_ArticleLookupFunction(const ItGrammarSynthesizer_ArticleLookupFunction&) = delete;
    ItGrammarSynthesizer_ArticleLookupFunction& operator=(const ItGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
