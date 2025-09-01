/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/GrammarSynthesizerFactory.hpp>

#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/DaGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/HiGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/NbGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/PtGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/SrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

typedef void (*addSemanticFeatures)(::inflection::dialog::SemanticFeatureModel& featureModel);

static const ::std::map<::inflection::util::ULocale, addSemanticFeatures>& GRAMMAR_SYNTHESIZER_MAP()
{
    static auto GRAMMAR_SYNTHESIZER_MAP_ = new ::std::map<::inflection::util::ULocale, addSemanticFeatures>({
        {::inflection::util::LocaleUtils::ARABIC(), &ArGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::DANISH(), &DaGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::GERMAN(), &DeGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::ENGLISH(), &EnGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::SPANISH(), &EsGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::FRENCH(), &FrGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::HEBREW(), &HeGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::HINDI(), &HiGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::ITALIAN(), &ItGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::KOREAN(), &KoGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::MALAYALAM(), &MlGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::NORWEGIAN(), &NbGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::DUTCH(), &NlGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::PORTUGUESE(), &PtGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::RUSSIAN(), &RuGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::SERBIAN(), &SrGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::SWEDISH(), &SvGrammarSynthesizer::addSemanticFeatures},
        {::inflection::util::LocaleUtils::TURKISH(), &TrGrammarSynthesizer::addSemanticFeatures},
    });
    return *npc(GRAMMAR_SYNTHESIZER_MAP_);
}

void GrammarSynthesizerFactory::addSemanticFeatures(const ::inflection::util::ULocale& locale, ::inflection::dialog::SemanticFeatureModel& featureModel)
{
    auto semanticFeatureAdder = GRAMMAR_SYNTHESIZER_MAP().find(locale);
    if (semanticFeatureAdder == GRAMMAR_SYNTHESIZER_MAP().end()) {
        semanticFeatureAdder = GRAMMAR_SYNTHESIZER_MAP().find(locale.getFallback());
    }
    if (semanticFeatureAdder != GRAMMAR_SYNTHESIZER_MAP().end()) {
        return semanticFeatureAdder->second(featureModel);
    }
}

static ::std::set<::inflection::util::ULocale>* initKnownLocales() {
    auto result = new ::std::set<::inflection::util::ULocale>();
    for (const auto& entry : GRAMMAR_SYNTHESIZER_MAP()) {
        if (!entry.first.getLanguage().empty()) {
            npc(result)->insert(entry.first);
        }
        // Skip the root locale
    }
    return result;
}

const ::std::set<::inflection::util::ULocale>& GrammarSynthesizerFactory::getKnownLocales()
{
    static auto KNOWN_LOCALES_ = initKnownLocales();
    return *npc(KNOWN_LOCALES_);
}

} // namespace inflection::grammar::synthesis
