/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::NlGrammarSynthesizer_DefinitenessLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    NlGrammarSynthesizer_DefinitenessLookupFunction();
    NlGrammarSynthesizer_DefinitenessLookupFunction(const NlGrammarSynthesizer_DefinitenessLookupFunction&) = delete;
    NlGrammarSynthesizer_DefinitenessLookupFunction& operator=(const NlGrammarSynthesizer_DefinitenessLookupFunction&) = delete;
};
