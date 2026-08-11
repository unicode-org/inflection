/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/KoGrammarSynthesizer.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_KoDisplayFunction.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_ParticleLookupFunction.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::EMPTY_SET()
{
    static auto EMPTY_SET_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet());
    return *npc(EMPTY_SET_);
}

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::NONLATIN_VOWELS()
{
    static auto NONLATIN_VOWELS_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::removeAll(new ::icu4cxx::UnicodeSet(::inflection::dictionary::PhraseProperties::DEFAULT_VOWELS_END().cloneAsThawed()), ::inflection::lang::StringFilterUtil::LATIN_SCRIPT()));
    return *npc(NONLATIN_VOWELS_);
}

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::VOWELS_WITH_RIEUL()
{
    static auto VOWELS_WITH_RIEUL_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::add(::inflection::util::UnicodeSetUtils::add(new ::icu4cxx::UnicodeSet(NONLATIN_VOWELS().cloneAsThawed()), u'\u1105'), u'\u11af'));
    return *npc(VOWELS_WITH_RIEUL_);
}

const icu4cxx::UnicodeSet& KoGrammarSynthesizer::DEFAULT_RIEUL_END()
{
    static auto DEFAULT_RIEUL_END_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::closeOver(npc(new ::icu4cxx::UnicodeSet(u"[l\u1105\u11af]")), USET_CASE_INSENSITIVE));
    return *npc(DEFAULT_RIEUL_END_);
}
void KoGrammarSynthesizer::addSemanticFeatures(::inflection::dialog::SemanticFeatureModel& featureModel)
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

} // namespace inflection::grammar::synthesis
