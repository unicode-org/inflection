/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 1/31/24.
//
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DictionaryLookupFunction.hpp>
#include <string>

class inflection::grammar::synthesis::ArGrammarSynthesizer_DefinitenessLookupFunction
    : public ::inflection::dialog::DictionaryLookupFunction
{
public:
    typedef ::inflection::dialog::DictionaryLookupFunction super;

private:
    ::std::set<std::u16string_view> possessivePronouns {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    explicit ArGrammarSynthesizer_DefinitenessLookupFunction();
    ArGrammarSynthesizer_DefinitenessLookupFunction(const ArGrammarSynthesizer_DefinitenessLookupFunction&) = delete;
    ArGrammarSynthesizer_DefinitenessLookupFunction& operator=(const ArGrammarSynthesizer_DefinitenessLookupFunction&) = delete;

private:
    int64_t properNounMask {  };
    int64_t definitenessMask {  };
    int64_t definitePOSMask {  };
};
