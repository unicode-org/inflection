/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>

class inflection::grammar::synthesis::HeGrammarSynthesizer_WithConditionalHyphen
    : public virtual ::inflection::dialog::DefaultFeatureFunction
{
public:
    typedef ::inflection::dialog::DefaultFeatureFunction super;
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

public:
    HeGrammarSynthesizer_WithConditionalHyphen();
    HeGrammarSynthesizer_WithConditionalHyphen(const HeGrammarSynthesizer_WithConditionalHyphen&) = delete;
    HeGrammarSynthesizer_WithConditionalHyphen& operator=(const HeGrammarSynthesizer_WithConditionalHyphen&) = delete;
};
