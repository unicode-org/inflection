/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_ParticleResolver.hpp>
#include <string>
#include <set>
#include <vector>
#include <map>

class inflection::grammar::synthesis::MlGrammarSynthesizer_MlDisplayFunction : public virtual ::inflection::dialog::DefaultDisplayFunction {
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& definitenessFeature;
    const ::inflection::dialog::SemanticFeature& adjectivalFeature;

    std::map<std::u16string, MlGrammarSynthesizer_ParticleResolver> particleMap;

public:
    ::inflection::dialog::DisplayValue* getDisplayValue(
        const dialog::SemanticFeatureModel_DisplayData& displayData,
        const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
        bool enableInflectionGuess) const override;

    explicit MlGrammarSynthesizer_MlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~MlGrammarSynthesizer_MlDisplayFunction() override;

    MlGrammarSynthesizer_MlDisplayFunction(MlGrammarSynthesizer_MlDisplayFunction&) = delete;
    MlGrammarSynthesizer_MlDisplayFunction& operator=(const MlGrammarSynthesizer_MlDisplayFunction&) = delete;

private:
    friend class MlGrammarSynthesizer;
};

