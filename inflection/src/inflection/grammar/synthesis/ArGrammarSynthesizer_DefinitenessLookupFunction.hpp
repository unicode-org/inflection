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
    ::std::map<int32_t, ::std::u16string_view> pronounMap {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    ArGrammarSynthesizer_DefinitenessLookupFunction(const ::std::map<int32_t, ::std::u16string_view>& pronounMap);

private:
    int64_t properNounMask = 0;
    int64_t definitenessMask = 0;
    int64_t definitePOSMask = 0;
};
