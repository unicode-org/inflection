/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer_ParticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>

namespace morphuntion::grammar::synthesis {

KoGrammarSynthesizer_ParticleLookupFunction::KoGrammarSynthesizer_ParticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const icu4cxx::UnicodeSet& vowelSet, const ::std::u16string& vowelParticle, const ::std::u16string& consonantParticle)
    : super(model, true, false)
    , particleResolver(vowelSet, vowelParticle, consonantParticle)
{
}

morphuntion::dialog::SpeakableString* KoGrammarSynthesizer_ParticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    return createPostposition(displayValue, particleResolver.getParticleValue(displayValue.getDisplayString(), true));
}

} // namespace morphuntion::grammar::synthesis
