/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/GrammarSynthesizerFactory.hpp>

#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/BnGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/CsGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/DaGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/DeGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/EnGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/EsGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/FiGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/FrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GuGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/HeGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/HiGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/KnGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/MrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/NbGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/PaGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/PlGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/PtGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/RoGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/RuGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/SrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/SvGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/TaGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/TeGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/TrGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/UkGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/UrGrammarSynthesizer.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/util/ULocale.hpp>

namespace inflection::grammar::synthesis {

static constexpr struct {
    const char *language;
    void (*semanticFeatureAdder)(::inflection::dialog::SemanticFeatureModel& featureModel);
} GRAMMAR_SYNTHESIZER_MAP[] = {
    // It's very important to keep this in sorted order for the binary search
    {"ar", &ArGrammarSynthesizer::addSemanticFeatures},
    {"bn", &BnGrammarSynthesizer::addSemanticFeatures},
    {"cs", &CsGrammarSynthesizer::addSemanticFeatures},
    {"da", &DaGrammarSynthesizer::addSemanticFeatures},
    {"de", &DeGrammarSynthesizer::addSemanticFeatures},
    {"en", &EnGrammarSynthesizer::addSemanticFeatures},
    {"es", &EsGrammarSynthesizer::addSemanticFeatures},
    {"fi", &FiGrammarSynthesizer::addSemanticFeatures},
    {"fr", &FrGrammarSynthesizer::addSemanticFeatures},
    {"gu", &GuGrammarSynthesizer::addSemanticFeatures},
    {"he", &HeGrammarSynthesizer::addSemanticFeatures},
    {"hi", &HiGrammarSynthesizer::addSemanticFeatures},
    {"it", &ItGrammarSynthesizer::addSemanticFeatures},
    {"kn", &KnGrammarSynthesizer::addSemanticFeatures},
    {"ko", &KoGrammarSynthesizer::addSemanticFeatures},
    {"ml", &MlGrammarSynthesizer::addSemanticFeatures},
    {"mr", &MrGrammarSynthesizer::addSemanticFeatures},
    {"nb", &NbGrammarSynthesizer::addSemanticFeatures},
    {"nl", &NlGrammarSynthesizer::addSemanticFeatures},
    {"pa", &PaGrammarSynthesizer::addSemanticFeatures},
    {"pl", &PlGrammarSynthesizer::addSemanticFeatures},
    {"pt", &PtGrammarSynthesizer::addSemanticFeatures},
    {"ro", &RoGrammarSynthesizer::addSemanticFeatures},
    {"ru", &RuGrammarSynthesizer::addSemanticFeatures},
    {"sr", &SrGrammarSynthesizer::addSemanticFeatures},
    {"sv", &SvGrammarSynthesizer::addSemanticFeatures},
    {"ta", &TaGrammarSynthesizer::addSemanticFeatures},
    {"te", &TeGrammarSynthesizer::addSemanticFeatures},
    {"tr", &TrGrammarSynthesizer::addSemanticFeatures},
    {"uk", &UkGrammarSynthesizer::addSemanticFeatures},
    {"ur", &UrGrammarSynthesizer::addSemanticFeatures},
};

void GrammarSynthesizerFactory::addSemanticFeatures(const ::inflection::util::ULocale& locale, ::inflection::dialog::SemanticFeatureModel& featureModel)
{
    auto localeItr(locale);
    while (!localeItr.getLanguage().empty()) {
        auto *entry = inflection::util::ArrayUtils::searchSorted<GRAMMAR_SYNTHESIZER_MAP>(localeItr.getName(),
            [](const auto& item) { return item.language; });
        if (entry != nullptr) {
            return entry->semanticFeatureAdder(featureModel);
        }
        localeItr = localeItr.getFallback();
    }
}

::std::set<::inflection::util::ULocale> GrammarSynthesizerFactory::getKnownLocales()
{
    // This is used for testing purposes. Caching the result for speed is not a requirement.
    std::set<::inflection::util::ULocale> result;
    for (const auto& entry : GRAMMAR_SYNTHESIZER_MAP) {
        result.emplace(entry.language);
    }
    return result;
}

} // namespace inflection::grammar::synthesis
