/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/DefaultDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/fwd.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer_ParticleLookupFunction.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer_ParticleResolver.hpp>
#include <string>
#include <set>
#include <vector>

class morphuntion::grammar::synthesis::KoGrammarSynthesizer_KoDisplayFunction
    : public virtual ::morphuntion::dialog::DefaultDisplayFunction
{
public:
    typedef ::morphuntion::dialog::DefaultDisplayFunction super;

private:
    const ::morphuntion::dialog::SemanticFeature* caseFeature {  };
    const ::morphuntion::dialog::SemanticFeature* clusivityFeature {  };
    const ::morphuntion::dialog::SemanticFeature* adjectivalFeature {  };
    std::map<std::u16string, KoGrammarSynthesizer_ParticleResolver> particleMap {  };

public:
    ::morphuntion::dialog::SemanticFeatureModel_DisplayValue * getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const override;

public:
    explicit KoGrammarSynthesizer_KoDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model);
    ~KoGrammarSynthesizer_KoDisplayFunction() override;

public:
    KoGrammarSynthesizer_KoDisplayFunction(KoGrammarSynthesizer_KoDisplayFunction&) = delete;
    KoGrammarSynthesizer_KoDisplayFunction& operator=(const KoGrammarSynthesizer_KoDisplayFunction&) = delete;

private:
    friend class KoGrammarSynthesizer;
};
