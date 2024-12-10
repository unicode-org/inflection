/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 1/31/24.
//
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <string>

class morphuntion::grammar::synthesis::ArGrammarSynthesizer_DefinitenessLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

private:
    ::std::map<int32_t, ::std::u16string_view> pronounMap {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    ArGrammarSynthesizer_DefinitenessLookupFunction(const ::std::map<int32_t, ::std::u16string_view>& pronounMap);

private:
    int64_t properNounMask = 0;
    int64_t definitenessMask = 0;
    int64_t definitePOSMask = 0;
};
