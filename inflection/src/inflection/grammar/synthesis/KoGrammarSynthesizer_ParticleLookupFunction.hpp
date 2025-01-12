/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <icu4cxx/UnicodeSet.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/DefaultArticleLookupFunction.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_ParticleResolver.hpp>

class inflection::grammar::synthesis::KoGrammarSynthesizer_ParticleLookupFunction
    : public ::inflection::dialog::DefaultArticleLookupFunction
{
public:
    typedef ::inflection::dialog::DefaultArticleLookupFunction super;

private:
    KoGrammarSynthesizer_ParticleResolver particleResolver;

public:
    ::inflection::dialog::SpeakableString* getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const override;

    KoGrammarSynthesizer_ParticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const icu4cxx::UnicodeSet& vowelSet, const ::std::u16string& vowelParticle, const ::std::u16string& consonantParticle);
    KoGrammarSynthesizer_ParticleLookupFunction(const KoGrammarSynthesizer_ParticleLookupFunction&) = delete;
    KoGrammarSynthesizer_ParticleLookupFunction& operator=(const KoGrammarSynthesizer_ParticleLookupFunction&) = delete;
};
