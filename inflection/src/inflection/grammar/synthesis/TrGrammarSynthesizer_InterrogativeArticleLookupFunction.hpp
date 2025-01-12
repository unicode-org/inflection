/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>

class inflection::grammar::synthesis::TrGrammarSynthesizer_InterrogativeArticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    const TrGrammarSynthesizer_TrDisplayFunction* displayFunction {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    TrGrammarSynthesizer_InterrogativeArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const TrGrammarSynthesizer_TrDisplayFunction* displayFunctionUtility);
    TrGrammarSynthesizer_InterrogativeArticleLookupFunction(const TrGrammarSynthesizer_InterrogativeArticleLookupFunction&) = delete;
    TrGrammarSynthesizer_InterrogativeArticleLookupFunction& operator=(const TrGrammarSynthesizer_InterrogativeArticleLookupFunction&) = delete;
};
