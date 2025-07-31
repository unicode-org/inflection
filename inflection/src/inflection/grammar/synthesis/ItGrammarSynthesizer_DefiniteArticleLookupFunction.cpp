/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_DefiniteArticleLookupFunction.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>

namespace inflection::grammar::synthesis {

ItGrammarSynthesizer_DefiniteArticleLookupFunction::ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& simplePreposition, const ::std::u16string& singularMasculine, const ::std::u16string& singularMasculineWithConst, const ::std::u16string& singularFeminine, const ::std::u16string& singularWithVowel, const ::std::u16string& pluralMasculine, const ::std::u16string& pluralMasculineWithConst, const ::std::u16string& pluralFeminine)
    : super(model, derivedSemanticName)
    , simplePreposition(simplePreposition)
    , singularMasculine(singularMasculine)
    , singularMasculineWithConst(singularMasculineWithConst)
    , singularFeminine(singularFeminine)
    , singularWithVowel(singularWithVowel)
    , pluralMasculine(pluralMasculine)
    , pluralMasculineWithConst(pluralMasculineWithConst)
    , pluralFeminine(pluralFeminine)
{
}

ItGrammarSynthesizer_DefiniteArticleLookupFunction::ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ItGrammarSynthesizer_DefiniteArticleLookupFunction& other)
    : ItGrammarSynthesizer_DefiniteArticleLookupFunction(model, derivedSemanticName, other.simplePreposition, other.singularMasculine, other.singularMasculineWithConst, other.singularFeminine, other.singularWithVowel, other.pluralMasculine, other.pluralMasculineWithConst, other.pluralFeminine)
{
}

inflection::dialog::SpeakableString* ItGrammarSynthesizer_DefiniteArticleLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Number numberValue, ItGrammarSynthesizer::Gender genderValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!wantArticle) {
        return createPreposition(displayValue, simplePreposition);
    }
    if (genderValue == ItGrammarSynthesizer::Gender::feminine) {
        if (numberValue == ItGrammarSynthesizer::Number::plural) {
            return createPreposition(displayValue, pluralFeminine);
        }
        if (ItGrammarSynthesizer::startsWithVowelForElision(displayString)) {
            return createPreposition(displayValue, singularWithVowel);
        }
        return createPreposition(displayValue, singularFeminine);
    }
    if (numberValue == ItGrammarSynthesizer::Number::plural) {
        if (::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ITALIAN(), displayString) || ItGrammarSynthesizer::startsWithConsonantSubset(displayString)) {
            return createPreposition(displayValue, pluralMasculineWithConst);
        }
        return createPreposition(displayValue, pluralMasculine);
    }
    if (ItGrammarSynthesizer::startsWithVowelForElision(displayString)) {
        return createPreposition(displayValue, singularWithVowel);
    }
    if (ItGrammarSynthesizer::startsWithConsonantSubset(displayString)) {
        return createPreposition(displayValue, singularMasculineWithConst);
    }
    if (numberValue == ItGrammarSynthesizer::Number::singular && genderValue == ItGrammarSynthesizer::Gender::masculine) {
        return createPreposition(displayValue, singularMasculine);
    }
    return createPreposition(displayValue, ::std::u16string());
}

} // namespace inflection::grammar::synthesis
