/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/KoGrammarSynthesizer_KoDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

static constexpr auto ADJECTIVAL = u"adjectival";

static constexpr const char16_t* PARTICLE_KEYS[] = {
    // This must be in sorted order.
    GrammemeConstants::CASE_ABLATIVE,       // "ablative"
    GrammemeConstants::CASE_ACCUSATIVE,     // "accusative"
    GrammemeConstants::CASE_COMITATIVE,     // "comitative"
    GrammemeConstants::CLUSIVITY_EXCLUSIVE,  // "exclusive"
    GrammemeConstants::CASE_GENITIVE,       // "genitive"
    GrammemeConstants::CLUSIVITY_INCLUSIVE,   // "inclusive"
    GrammemeConstants::CASE_INSTRUMENTAL,   // "instrumental"
    GrammemeConstants::ADJECTIVAL_PREDICATIVE, // "predicative"
    GrammemeConstants::CASE_VOCATIVE,       // "vocative"
};

KoGrammarSynthesizer_KoDisplayFunction::KoGrammarSynthesizer_KoDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , clusivityFeature(*npc(model.getFeature(GrammemeConstants::CLUSIVITY)))
    , adjectivalFeature(*npc(model.getFeature(ADJECTIVAL)))
    , particleResolvers({
            {KoGrammarSynthesizer::VOWELS_WITH_RIEUL(), u"로부터", u"으로부터"}, // ablative: something "from" source or origin
            {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"를", u"을"}, // accusative: object is accusative
            {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"와", u"과"}, // comitative: adding the and is comitative
            {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"는", u"은"}, // exclusive: topic is nominative exclusive / informative
            {KoGrammarSynthesizer::EMPTY_SET(), u"", u"의"}, // genitive
            {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"가", u"이"}, // inclusive: subject is nominative inclusive / more ongoing
            {KoGrammarSynthesizer::VOWELS_WITH_RIEUL(), u"로", u"으로"}, // instrumental: directional is instrumental
            {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"라", u"이라"}, // predicative: It's like saying "named" or "called"
            {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"야", u"아"}, // vocative: Another way to say hey! TODO add support for register.
    })
{
}

KoGrammarSynthesizer_KoDisplayFunction::~KoGrammarSynthesizer_KoDisplayFunction()
{
}

::inflection::dialog::DisplayValue *
KoGrammarSynthesizer_KoDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }
    ::std::u16string featureString = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature);

    std::u16string particle;
    if (featureString.empty()) {
        featureString = GrammarSynthesizerUtil::getFeatureValue(constraints, adjectivalFeature);
    }
    else if (featureString == GrammemeConstants::CASE_NOMINATIVE) {
        featureString = GrammarSynthesizerUtil::getFeatureValue(constraints, clusivityFeature);
        if (featureString.empty()) {
            // Default to topic or informative instead of ongoing.
            featureString = GrammemeConstants::CLUSIVITY_EXCLUSIVE;
        }
    }

    auto particleKeyEntry = inflection::util::ArrayUtils::searchSorted<PARTICLE_KEYS>(featureString,
            [](const auto& item) { return item; });
    ::std::u16string displayString(displayValue->getDisplayString());
    if (particleKeyEntry != nullptr && !displayString.empty()) {
        auto idx = particleKeyEntry - PARTICLE_KEYS;
        auto result(particleResolvers[idx].switchParticleValue(displayString, enableInflectionGuess));
        if (result.has_value()) {
            displayString = result.value();
        }
        else {
            return nullptr;
        }
    }
    else if (!enableInflectionGuess) {
        return nullptr;
    }

    return new ::inflection::dialog::DisplayValue(displayString, constraints);
}

} // namespace inflection::grammar::synthesis
