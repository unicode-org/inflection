/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_ParticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::grammar::synthesis {

KoGrammarSynthesizer_ParticleLookupFunction::KoGrammarSynthesizer_ParticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const icu4cxx::UnicodeSet& vowelSet, const ::std::u16string& vowelParticle, const ::std::u16string& consonantParticle)
    : super(model, true, false)
    , particleResolver(vowelSet, vowelParticle, consonantParticle)
{
}

inflection::dialog::SpeakableString* KoGrammarSynthesizer_ParticleLookupFunction::getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    return createPostposition(displayValue, particleResolver.getParticleValue(displayValue.getDisplayString(), true));
}

} // namespace inflection::grammar::synthesis
