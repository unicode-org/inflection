/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/GrammarSynthesizerFactory.hpp>

#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/DaGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/DeGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/EsGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/HiGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/NbGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/PtGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/SvGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

typedef void (*addSemanticFeatures)(::morphuntion::dialog::SemanticFeatureModel& featureModel);

static const ::std::map<::morphuntion::util::ULocale, addSemanticFeatures>& GRAMMAR_SYNTHESIZER_MAP()
{
    static auto GRAMMAR_SYNTHESIZER_MAP_ = new ::std::map<::morphuntion::util::ULocale, addSemanticFeatures>({
        {::morphuntion::util::LocaleUtils::ARABIC(), &ArGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::DANISH(), &DaGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::GERMAN(), &DeGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::ENGLISH(), &EnGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::SPANISH(), &EsGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::FRENCH(), &FrGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::HEBREW(), &HeGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::HINDI(), &HiGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::ITALIAN(), &ItGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::KOREAN(), &KoGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::NORWEGIAN(), &NbGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::DUTCH(), &NlGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::PORTUGUESE(), &PtGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::RUSSIAN(), &RuGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::SERBIAN(), &SrGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::SWEDISH(), &SvGrammarSynthesizer::addSemanticFeatures},
        {::morphuntion::util::LocaleUtils::TURKISH(), &TrGrammarSynthesizer::addSemanticFeatures},
    });
    return *npc(GRAMMAR_SYNTHESIZER_MAP_);
}

void GrammarSynthesizerFactory::addSemanticFeatures(const ::morphuntion::util::ULocale& locale, ::morphuntion::dialog::SemanticFeatureModel& featureModel)
{
    auto semanticFeatureAdder = GRAMMAR_SYNTHESIZER_MAP().find(locale);
    if (semanticFeatureAdder == GRAMMAR_SYNTHESIZER_MAP().end()) {
        semanticFeatureAdder = GRAMMAR_SYNTHESIZER_MAP().find(locale.getFallback());
    }
    if (semanticFeatureAdder != GRAMMAR_SYNTHESIZER_MAP().end()) {
        return semanticFeatureAdder->second(featureModel);
    }
}

static ::std::set<::morphuntion::util::ULocale>* initKnownLocales() {
    auto result = new ::std::set<::morphuntion::util::ULocale>();
    for (const auto& entry : GRAMMAR_SYNTHESIZER_MAP()) {
        if (!entry.first.getLanguage().empty()) {
            npc(result)->insert(entry.first);
        }
        // Skip the root locale
    }
    return result;
}

const ::std::set<::morphuntion::util::ULocale>& GrammarSynthesizerFactory::getKnownLocales()
{
    static auto KNOWN_LOCALES_ = initKnownLocales();
    return *npc(KNOWN_LOCALES_);
}

} // namespace morphuntion::grammar::synthesis
