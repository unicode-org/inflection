/*
 * Copyright 2023-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer_KoDisplayFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <morphuntion/grammar/synthesis/KoGrammarSynthesizer.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::grammar::synthesis {

static constexpr auto ADJECTIVAL = u"adjectival";
static constexpr auto PREDICATIVE = u"predicative";

KoGrammarSynthesizer_KoDisplayFunction::KoGrammarSynthesizer_KoDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super()
    , caseFeature(npc(model.getFeature(GrammemeConstants::CASE)))
    , clusivityFeature(npc(model.getFeature(GrammemeConstants::CLUSIVITY)))
    , adjectivalFeature(npc(model.getFeature(ADJECTIVAL)))
    , particleMap({
            {GrammemeConstants::CASE_ABLATIVE(), {KoGrammarSynthesizer::VOWELS_WITH_RIEUL(), u"로부터", u"으로부터"}}, // something "from" source or origin
            {GrammemeConstants::CASE_ACCUSATIVE(), {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"를", u"을"}}, // object is accusative
            {GrammemeConstants::CASE_COMITATIVE(), {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"와", u"과"}}, // adding the and is comitative
            {GrammemeConstants::CASE_GENITIVE(), {KoGrammarSynthesizer::EMPTY_SET(), u"", u"의"}},
            {GrammemeConstants::CLUSIVITY_INCLUSIVE(), {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"가", u"이"}}, // subject is nominative inclusive / more ongoing
            {GrammemeConstants::CLUSIVITY_EXCLUSIVE(), {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"는", u"은"}}, // topic is nominative exclusive / informative
            {GrammemeConstants::CASE_INSTRUMENTAL(), {KoGrammarSynthesizer::VOWELS_WITH_RIEUL(), u"로", u"으로"}}, // directional is instrumental
            {GrammemeConstants::CASE_VOCATIVE(), {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"야", u"아"}}, // Another way to say hey! TODO add support for register.
            {PREDICATIVE, {KoGrammarSynthesizer::NONLATIN_VOWELS(), u"라", u"이라"}}, // It's like saying "named" or "called"
    })
{
}

KoGrammarSynthesizer_KoDisplayFunction::~KoGrammarSynthesizer_KoDisplayFunction()
{
}

::morphuntion::dialog::SemanticFeatureModel_DisplayValue *
KoGrammarSynthesizer_KoDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool enableInflectionGuess) const
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
    else if (featureString == GrammemeConstants::CASE_NOMINATIVE()) {
        featureString = GrammarSynthesizerUtil::getFeatureValue(constraints, clusivityFeature);
        if (featureString.empty()) {
            // Default to topic or informative instead of ongoing.
            featureString = GrammemeConstants::CLUSIVITY_EXCLUSIVE();
        }
    }

    auto particleResolver(particleMap.find(featureString));
    ::std::u16string displayString(displayValue->getDisplayString());
    if (particleResolver != particleMap.end() && !displayString.empty()) {
        auto result(particleResolver->second.switchParticleValue(displayString, enableInflectionGuess));
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

    return new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, constraints);
}

} // namespace morphuntion::grammar::synthesis
