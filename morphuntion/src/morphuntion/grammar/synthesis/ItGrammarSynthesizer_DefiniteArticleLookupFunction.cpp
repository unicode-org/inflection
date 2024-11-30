/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_DefiniteArticleLookupFunction.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>

namespace morphuntion::grammar::synthesis {

ItGrammarSynthesizer_DefiniteArticleLookupFunction::ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ::std::u16string& simplePreposition, const ::std::u16string& singularMasculine, const ::std::u16string& singularMasculineWithConst, const ::std::u16string& singularFeminine, const ::std::u16string& singularWithVowel, const ::std::u16string& pluralMasculine, const ::std::u16string& pluralMasculineWithConst, const ::std::u16string& pluralFeminine)
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

ItGrammarSynthesizer_DefiniteArticleLookupFunction::ItGrammarSynthesizer_DefiniteArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName, const ItGrammarSynthesizer_DefiniteArticleLookupFunction& other)
    : ItGrammarSynthesizer_DefiniteArticleLookupFunction(model, derivedSemanticName, other.simplePreposition, other.singularMasculine, other.singularMasculineWithConst, other.singularFeminine, other.singularWithVowel, other.pluralMasculine, other.pluralMasculineWithConst, other.pluralFeminine)
{
}

morphuntion::dialog::SpeakableString* ItGrammarSynthesizer_DefiniteArticleLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, bool wantArticle, ItGrammarSynthesizer::Count countValue, ItGrammarSynthesizer::Gender genderValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!wantArticle) {
        return createPreposition(displayValue, simplePreposition);
    }
    if (genderValue == ItGrammarSynthesizer::Gender::feminine) {
        if (countValue == ItGrammarSynthesizer::Count::plural) {
            return createPreposition(displayValue, pluralFeminine);
        }
        if (ItGrammarSynthesizer::startsWithVowelForElision(displayString)) {
            return createPreposition(displayValue, singularWithVowel);
        }
        return createPreposition(displayValue, singularFeminine);
    }
    if (countValue == ItGrammarSynthesizer::Count::plural) {
        if (::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ITALIAN(), displayString) || ItGrammarSynthesizer::startsWithConsonantSubset(displayString)) {
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
    if (countValue == ItGrammarSynthesizer::Count::singular && genderValue == ItGrammarSynthesizer::Gender::masculine) {
        return createPreposition(displayValue, singularMasculine);
    }
    return createPreposition(displayValue, ::std::u16string());
}

} // namespace morphuntion::grammar::synthesis
