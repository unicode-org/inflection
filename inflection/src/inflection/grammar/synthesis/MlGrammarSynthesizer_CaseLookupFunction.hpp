/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/

#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultFeatureFunction.hpp>

#include <vector>
#include <string>
#include <string_view>
#include <utility>

namespace inflection::grammar::synthesis {

class MlGrammarSynthesizer_CaseLookupFunction
    : public ::inflection::dialog::DefaultFeatureFunction
{
public:
    typedef ::inflection::dialog::DefaultFeatureFunction super;

public:
    MlGrammarSynthesizer_CaseLookupFunction();
    ~MlGrammarSynthesizer_CaseLookupFunction() override = default;

    MlGrammarSynthesizer_CaseLookupFunction(const MlGrammarSynthesizer_CaseLookupFunction&) = delete;
    MlGrammarSynthesizer_CaseLookupFunction& operator=(const MlGrammarSynthesizer_CaseLookupFunction&) = delete;

    ::inflection::dialog::SpeakableString* getFeatureValue(
        const ::inflection::dialog::DisplayValue& displayValue,
        const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

private:
    const std::vector<std::pair<std::u16string_view, std::u16string>> m_suffixToCase_;
};

} // namespace inflection::grammar::synthesis