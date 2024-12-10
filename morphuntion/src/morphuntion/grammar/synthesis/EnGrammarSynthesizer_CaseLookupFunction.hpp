/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/dialog/DefaultArticleLookupFunction.hpp>
#include <set>
#include <string>

class morphuntion::grammar::synthesis::EnGrammarSynthesizer_CaseLookupFunction
    : public ::morphuntion::dialog::DefaultFeatureFunction
{
public:
    typedef ::morphuntion::dialog::DefaultFeatureFunction super;

private:
    ::std::set<::std::u16string> contractions {  };

public:
    ::morphuntion::dialog::SpeakableString* getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    EnGrammarSynthesizer_CaseLookupFunction();
};
