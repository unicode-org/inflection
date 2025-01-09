/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlDisplayFunction.hpp>

#include <inflection/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_EmptyConstraintInflectorPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveListInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>
#include <inflection/grammar/synthesis/NlGrammarSynthesizer_PossessivePattern.hpp>

namespace inflection::grammar::synthesis {

NlGrammarSynthesizer_NlDisplayFunction::NlGrammarSynthesizer_NlDisplayFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super()
    , nlInflector({new NlGrammarSynthesizer_EmptyConstraintInflectorPattern(),
                   new NlGrammarSynthesizer_PossessivePattern(model),
                   new NlGrammarSynthesizer_NlNounInflectionPattern(model),
                   new NlGrammarSynthesizer_NlAdjectiveInflectionPattern(model),
                   new NlGrammarSynthesizer_NlAdjectiveListInflectionPattern(model),
                   new NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern(model)}, true)
    , definiteArticleLookupFunction(model, true, u"de", u"het")
    , indefiniteArticleLookupFunction(model, nullptr, u"een")
    , definitenessDisplayFunction(model, &definiteArticleLookupFunction, NlGrammarSynthesizer::ARTICLE_DEFINITE, &indefiniteArticleLookupFunction, NlGrammarSynthesizer::ARTICLE_INDEFINITE)
{
}

::inflection::dialog::SemanticFeatureModel_DisplayValue * NlGrammarSynthesizer_NlDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
{
    ::std::u16string displayString;
    if (!displayData.getValues().empty()) {
        displayString = displayData.getValues()[0].getDisplayString();
    }
    if (displayString.empty()) {
        return nullptr;
    }
    auto inflectionResult(nlInflector.inflect(displayString, constraints));
    if (!inflectionResult.empty()) {
        displayString = inflectionResult;
    }
    return definitenessDisplayFunction.addDefiniteness(new ::inflection::dialog::SemanticFeatureModel_DisplayValue(displayString, constraints), constraints);
}

} // namespace inflection::grammar::synthesis
