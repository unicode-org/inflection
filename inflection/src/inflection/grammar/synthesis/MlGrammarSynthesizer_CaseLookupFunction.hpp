/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>
#include <set>
#include <string>

class inflection::grammar::synthesis::MlGrammarSynthesizer_CaseLookupFunction
    : public ::inflection::dialog::DefaultFeatureFunction
{
public:
    typedef ::inflection::dialog::DefaultFeatureFunction super;

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    MlGrammarSynthesizer_CaseLookupFunction();
    MlGrammarSynthesizer_CaseLookupFunction(const MlGrammarSynthesizer_CaseLookupFunction&) = delete;
    MlGrammarSynthesizer_CaseLookupFunction& operator=(const MlGrammarSynthesizer_CaseLookupFunction&) = delete;
};

