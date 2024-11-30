/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::grammar::synthesis {

ArGrammarSynthesizer_ArticleLookupFunction::ArGrammarSynthesizer_ArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model)
    : super(model, true, false)
    , numberPronounFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_NUMBER)))
    , genderPronounFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_GENDER)))
    , personPronounFeature(*npc(model.getFeature(GrammemeConstants::PERSON)))
{
}

morphuntion::dialog::SpeakableString* ArGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = ArGrammarSynthesizer::getPronounNumber(DefaultArticleLookupFunction::getFeatureValue(constraints, numberPronounFeature));
    auto genderValue = ArGrammarSynthesizer::getPronounGender(DefaultArticleLookupFunction::getFeatureValue(constraints, genderPronounFeature));
    auto personValue = ArGrammarSynthesizer::getPerson(DefaultArticleLookupFunction::getFeatureValue(constraints, personPronounFeature));
    return getArticle(displayValue, countValue, genderValue, personValue);
}

} // namespace morphuntion::grammar::synthesis
