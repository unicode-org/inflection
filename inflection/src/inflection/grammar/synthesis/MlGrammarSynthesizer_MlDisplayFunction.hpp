/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <string>
#include <map>

class inflection::grammar::synthesis::MlGrammarSynthesizer_MlDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction {
public:
    using super = ::inflection::dialog::DefaultDisplayFunction;

private:
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& numberFeature;
    const ::inflection::dialog::SemanticFeature& genderFeature;
    const ::inflection::dialog::SemanticFeature& posFeature;
    const ::inflection::dialog::SemanticFeature& formalityFeature;
    const ::inflection::dialog::SemanticFeature& clusivityFeature;
    const ::inflection::dialog::SemanticFeature& personFeature;
    const ::inflection::dialog::SemanticFeature& tenseFeature;
    const ::inflection::dialog::SemanticFeature& moodFeature;

public:
    ::inflection::dialog::DisplayValue* getDisplayValue(
        const ::inflection::dialog::SemanticFeatureModel_DisplayData& displayData,
        const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
        bool enableInflectionGuess) const override;

    explicit MlGrammarSynthesizer_MlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~MlGrammarSynthesizer_MlDisplayFunction() override;

    MlGrammarSynthesizer_MlDisplayFunction(MlGrammarSynthesizer_MlDisplayFunction&) = delete;
    MlGrammarSynthesizer_MlDisplayFunction& operator=(const MlGrammarSynthesizer_MlDisplayFunction&) = delete;

private:
    friend class MlGrammarSynthesizer;
};
