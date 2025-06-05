/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_MlDisplayFunction.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/grammar/synthesis/GrammarSynthesizerUtil.hpp>
#include <inflection/grammar/synthesis/MlGrammarSynthesizer.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::grammar::synthesis {

static constexpr auto ADJECTIVAL = u"adjectival";
static constexpr auto PREDICATIVE = u"predicative";

MlGrammarSynthesizer_MlDisplayFunction::MlGrammarSynthesizer_MlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , caseFeature(*npc(model.getFeature(GrammemeConstants::CASE)))
    , definitenessFeature(*npc(model.getFeature(GrammemeConstants::DEFINITENESS)))
    , adjectivalFeature(*npc(model.getFeature(ADJECTIVAL)))
    , particleMap({
        {GrammemeConstants::CASE_ACCUSATIVE(),      {MlGrammarSynthesizer::FINAL_VOWELS(), u"യെ", u"നെ"}},
        {GrammemeConstants::CASE_DATIVE(),          {MlGrammarSynthesizer::FINAL_VOWELS(), u"ക്ക്", u"നു"}},
        {GrammemeConstants::CASE_GENITIVE(),        {MlGrammarSynthesizer::EMPTY_SET(),     u"",    u"ന്റെ"}},
        {GrammemeConstants::CASE_LOCATIVE(),        {MlGrammarSynthesizer::FINAL_VOWELS(), u"യിൽ", u"ൽ"}},
        {GrammemeConstants::CASE_ABLATIVE(),        {MlGrammarSynthesizer::FINAL_VOWELS(), u"ഇൽനിന്ന്", u"ൽനിന്ന്"}},
        {GrammemeConstants::CASE_INSTRUMENTAL(),    {MlGrammarSynthesizer::FINAL_VOWELS(), u"ഉപയോഗിച്ച്", u"കൊണ്ട്"}},
        {GrammemeConstants::CASE_VOCATIVE(),        {MlGrammarSynthesizer::FINAL_VOWELS(), u"ആ", u"േ"}},
        {PREDICATIVE,                               {MlGrammarSynthesizer::FINAL_VOWELS(), u"ആണ്", u"ഇാണ്"}}
    }) {}

MlGrammarSynthesizer_MlDisplayFunction::~MlGrammarSynthesizer_MlDisplayFunction() = default;

::inflection::dialog::DisplayValue*
MlGrammarSynthesizer_MlDisplayFunction::getDisplayValue(
    const dialog::SemanticFeatureModel_DisplayData& displayData,
    const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints,
    bool enableInflectionGuess) const
{
    const auto displayValue = GrammarSynthesizerUtil::getTheBestDisplayValue(displayData, constraints);
    if (displayValue == nullptr) {
        return nullptr;
    }

    std::u16string featureString = GrammarSynthesizerUtil::getFeatureValue(constraints, caseFeature);
    std::u16string particle;

    if (featureString.empty()) {
        featureString = GrammarSynthesizerUtil::getFeatureValue(constraints, adjectivalFeature);
    }

    auto particleResolver = particleMap.find(featureString);
    std::u16string displayString(displayValue->getDisplayString());

    if (particleResolver != particleMap.end() && !displayString.empty()) {
        auto result = particleResolver->second.switchParticleValue(displayString, enableInflectionGuess);
        if (result.has_value()) {
            displayString = result.value();
        } else {
            return nullptr;
        }
    } else if (!enableInflectionGuess) {
        return nullptr;
    }

    return new ::inflection::dialog::DisplayValue(displayString, constraints);
}

} // namespace inflection::grammar::synthesis

