/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>

namespace inflection::grammar::synthesis {

class MlGrammarSynthesizer_CaseLookupFunction
    : public ::inflection::dialog::DefaultFeatureFunction
{
public:
    typedef ::inflection::dialog::DefaultFeatureFunction super;

public:
    MlGrammarSynthesizer_CaseLookupFunction() = default;
    ~MlGrammarSynthesizer_CaseLookupFunction() override = default;

    MlGrammarSynthesizer_CaseLookupFunction(const MlGrammarSynthesizer_CaseLookupFunction&) = delete;
    MlGrammarSynthesizer_CaseLookupFunction& operator=(const MlGrammarSynthesizer_CaseLookupFunction&) = delete;

    ::inflection::dialog::SpeakableString* getFeatureValue(
        const ::inflection::dialog::DisplayValue& displayValue,
        const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;
};

} // namespace inflection::grammar::synthesis