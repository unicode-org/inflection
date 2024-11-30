/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer_KoDisplayFunction.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer_ParticleLookupFunction.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::EMPTY_SET()
{
    static auto EMPTY_SET_ = ::morphuntion::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet());
    return *npc(EMPTY_SET_);
}

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::NONLATIN_VOWELS()
{
    static auto NONLATIN_VOWELS_ = ::morphuntion::util::UnicodeSetUtils::freeze(::morphuntion::util::UnicodeSetUtils::removeAll(new ::icu4cxx::UnicodeSet(::morphuntion::dictionary::PhraseProperties::DEFAULT_VOWELS_END().cloneAsThawed()), ::morphuntion::lang::StringFilterUtil::LATIN_SCRIPT()));
    return *npc(NONLATIN_VOWELS_);
}

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::VOWELS_WITH_RIEUL()
{
    static auto VOWELS_WITH_RIEUL_ = ::morphuntion::util::UnicodeSetUtils::freeze(::morphuntion::util::UnicodeSetUtils::add(::morphuntion::util::UnicodeSetUtils::add(new ::icu4cxx::UnicodeSet(NONLATIN_VOWELS().cloneAsThawed()), u'\u1105'), u'\u11af'));
    return *npc(VOWELS_WITH_RIEUL_);
}

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::DEFAULT_RIEUL_END()
{
    static auto DEFAULT_RIEUL_END_ = ::morphuntion::util::UnicodeSetUtils::freeze(::morphuntion::util::UnicodeSetUtils::closeOver(npc(new ::icu4cxx::UnicodeSet(u"[l\u1105\u11af]")), USET_CASE_INSENSITIVE));
    return *npc(DEFAULT_RIEUL_END_);
}
void KoGrammarSynthesizer::addSemanticFeatures(::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
//    featureModel.putDefaultFeatureFunctionByName(u"eu", new KoGrammarSynthesizer_ParticleLookupFunction(featureModel, VOWELS_WITH_RIEUL(), ::std::u16string(), u"으"));
//    featureModel.putDefaultFeatureFunctionByName(u"yi", new KoGrammarSynthesizer_ParticleLookupFunction(featureModel, NONLATIN_VOWELS(), ::std::u16string(), u"이"));
//    featureModel.putDefaultFeatureFunctionByName(u"topic", new KoGrammarSynthesizer_ParticleLookupFunction(featureModel, NONLATIN_VOWELS(), u"는", u"은"));
//    featureModel.putDefaultFeatureFunctionByName(u"subject", new KoGrammarSynthesizer_ParticleLookupFunction(featureModel, NONLATIN_VOWELS(), u"가", u"이"));
//    featureModel.putDefaultFeatureFunctionByName(u"object", new KoGrammarSynthesizer_ParticleLookupFunction(featureModel, NONLATIN_VOWELS(), u"를", u"을"));
//    featureModel.putDefaultFeatureFunctionByName(u"directional", new KoGrammarSynthesizer_ParticleLookupFunction(featureModel, VOWELS_WITH_RIEUL(), u"로", u"으로"));
//    featureModel.putDefaultFeatureFunctionByName(u"addAnd", new KoGrammarSynthesizer_ParticleLookupFunction(featureModel, NONLATIN_VOWELS(), u"와", u"과"));
    featureModel.setDefaultDisplayFunction(new KoGrammarSynthesizer_KoDisplayFunction(featureModel));
}

} // namespace morphuntion::grammar::synthesis
