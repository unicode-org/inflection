/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlDisplayFunction.hpp>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayData.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_EmptyConstraintInflectorPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveListInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlAdjectiveNounInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_NlNounInflectionPattern.hpp>
#include <morphuntion/grammar/synthesis/NlGrammarSynthesizer_PossessivePattern.hpp>

namespace morphuntion::grammar::synthesis {

NlGrammarSynthesizer_NlDisplayFunction::NlGrammarSynthesizer_NlDisplayFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
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

::morphuntion::dialog::SemanticFeatureModel_DisplayValue * NlGrammarSynthesizer_NlDisplayFunction::getDisplayValue(const dialog::SemanticFeatureModel_DisplayData &displayData, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string> &constraints, bool /*enableInflectionGuess*/) const
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
    return definitenessDisplayFunction.addDefiniteness(new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(displayString, constraints), constraints);
}

} // namespace morphuntion::grammar::synthesis
