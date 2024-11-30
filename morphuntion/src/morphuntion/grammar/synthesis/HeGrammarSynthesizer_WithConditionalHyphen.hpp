/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultFeatureFunction.hpp>

class morphuntion::grammar::synthesis::HeGrammarSynthesizer_WithConditionalHyphen
    : public virtual ::morphuntion::dialog::DefaultFeatureFunction
{
public:
    typedef ::morphuntion::dialog::DefaultFeatureFunction super;
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    HeGrammarSynthesizer_WithConditionalHyphen();
};
