/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <set>
#include <string>

class inflection::grammar::synthesis::EnGrammarSynthesizer_CaseLookupFunction
    : public ::inflection::dialog::DefaultFeatureFunction
{
public:
    typedef ::inflection::dialog::DefaultFeatureFunction super;

private:
    ::std::set<::std::u16string> contractions {  };

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    EnGrammarSynthesizer_CaseLookupFunction();
};
