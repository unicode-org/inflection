/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <map>
#include <string>

class inflection::grammar::synthesis::RuGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };

protected: /* package */
    virtual ::inflection::dialog::SpeakableString* getArticle(const ::inflection::dialog::DisplayValue& displayValue) const = 0;

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

public: /* package */
    RuGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName);
    RuGrammarSynthesizer_ArticleLookupFunction(const RuGrammarSynthesizer_ArticleLookupFunction&) = delete;
    RuGrammarSynthesizer_ArticleLookupFunction& operator=(const RuGrammarSynthesizer_ArticleLookupFunction&) = delete;
};
