/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>

class morphuntion::grammar::synthesis::TrGrammarSynthesizer_InterrogativeArticleLookupFunction
    : public ::morphuntion::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::morphuntion::dialog::DefaultArticleLookupFunction super;

private:
    const TrGrammarSynthesizer_TrDisplayFunction* displayFunction {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    TrGrammarSynthesizer_InterrogativeArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const TrGrammarSynthesizer_TrDisplayFunction* displayFunctionUtility);
};
