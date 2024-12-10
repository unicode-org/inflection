/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

RuGrammarSynthesizer_ArticleLookupFunction::RuGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, !derivedSemanticName.empty(), true)
    , derivedArticleFeature(!derivedSemanticName.empty() ? model.getFeature(derivedSemanticName) : static_cast<const ::morphuntion::dialog::SemanticFeature*>(nullptr))
{
}

morphuntion::dialog::SpeakableString* RuGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    if (derivedArticleFeature != nullptr) {
        auto featureValue = displayValue.getFeatureValue(*npc(derivedArticleFeature));
        if (featureValue != nullptr) {
            return createPreposition(displayValue, *npc(featureValue));
        }
    }
    return getArticle(displayValue);
}

} // namespace morphuntion::grammar::synthesis
