/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/DefaultDisplayFunction.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_ParticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_ParticleResolver.hpp>
#include <string>
#include <set>
#include <vector>

class inflection::grammar::synthesis::KoGrammarSynthesizer_KoDisplayFunction
    : public virtual ::inflection::dialog::DefaultDisplayFunction
{
public:
    typedef ::inflection::dialog::DefaultDisplayFunction super;

private:
    const ::inflection::dialog::SemanticFeature& caseFeature;
    const ::inflection::dialog::SemanticFeature& clusivityFeature;
    const ::inflection::dialog::SemanticFeature& adjectivalFeature;
    std::map<std::u16string, KoGrammarSynthesizer_ParticleResolver> particleMap {  };

public:
    ::inflection::dialog::DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public:
    explicit KoGrammarSynthesizer_KoDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model);
    ~KoGrammarSynthesizer_KoDisplayFunction() override;

public:
    KoGrammarSynthesizer_KoDisplayFunction(KoGrammarSynthesizer_KoDisplayFunction&) = delete;
    KoGrammarSynthesizer_KoDisplayFunction& operator=(const KoGrammarSynthesizer_KoDisplayFunction&) = delete;

private:
    friend class KoGrammarSynthesizer;
};
