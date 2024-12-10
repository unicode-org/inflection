/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DictionaryLookupFunction.hpp>
#include <string>

class morphuntion::grammar::synthesis::NlGrammarSynthesizer_DefinitenessLookupFunction
    : public ::morphuntion::dialog::DictionaryLookupFunction
{
public:
    typedef ::morphuntion::dialog::DictionaryLookupFunction super;

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    NlGrammarSynthesizer_DefinitenessLookupFunction();
};
