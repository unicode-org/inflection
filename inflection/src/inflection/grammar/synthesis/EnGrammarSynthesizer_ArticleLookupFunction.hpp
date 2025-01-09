/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_CountLookupFunction.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::EnGrammarSynthesizer_ArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const ::inflection::dialog::SemanticFeature* derivedArticleFeature {  };
    EnGrammarSynthesizer_CountLookupFunction countLookupFunction {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    EnGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const char16_t* derivedSemanticName);
};
