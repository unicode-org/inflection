/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ArGrammarSynthesizer_ArticleLookupFunction.hpp>

#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/ArGrammarSynthesizer.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>

namespace inflection::grammar::synthesis {

ArGrammarSynthesizer_ArticleLookupFunction::ArGrammarSynthesizer_ArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model)
    : super(model, true, false)
    , numberPronounFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_NUMBER)))
    , genderPronounFeature(*npc(model.getFeature(ArGrammarSynthesizer::PRONOUN_GENDER)))
    , personPronounFeature(*npc(model.getFeature(GrammemeConstants::PERSON)))
{
}

inflection::dialog::SpeakableString* ArGrammarSynthesizer_ArticleLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& constraints) const
{
    auto countValue = ArGrammarSynthesizer::getPronounNumber(DefaultArticleLookupFunction::getFeatureValue(constraints, numberPronounFeature));
    auto genderValue = ArGrammarSynthesizer::getPronounGender(DefaultArticleLookupFunction::getFeatureValue(constraints, genderPronounFeature));
    auto personValue = ArGrammarSynthesizer::getPerson(DefaultArticleLookupFunction::getFeatureValue(constraints, personPronounFeature));
    return getArticle(displayValue, countValue, genderValue, personValue);
}

} // namespace inflection::grammar::synthesis
