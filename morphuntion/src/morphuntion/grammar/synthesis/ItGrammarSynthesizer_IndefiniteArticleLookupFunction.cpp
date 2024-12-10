/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer_IndefiniteArticleLookupFunction.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/ItGrammarSynthesizer.hpp>

namespace morphuntion::grammar::synthesis {


ItGrammarSynthesizer_IndefiniteArticleLookupFunction::ItGrammarSynthesizer_IndefiniteArticleLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

morphuntion::dialog::SpeakableString* ItGrammarSynthesizer_IndefiniteArticleLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, bool /*wantArticle*/, ItGrammarSynthesizer::Count countValue, ItGrammarSynthesizer::Gender genderValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (genderValue == ItGrammarSynthesizer::Gender::feminine) {
        if (countValue == ItGrammarSynthesizer::Count::plural) {
            return createPreposition(displayValue, u"delle ");
        }
        if (::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ITALIAN(), displayString)) {
            return createPreposition(displayValue, u"un’");
        }
        return createPreposition(displayValue, u"una ");
    }
    if (countValue == ItGrammarSynthesizer::Count::plural) {
        if (::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::ITALIAN(), displayString) || ItGrammarSynthesizer::startsWithConsonantSubset(displayString)) {
            return createPreposition(displayValue, u"degli ");
        }
        return createPreposition(displayValue, u"dei ");
    }
    if (ItGrammarSynthesizer::startsWithConsonantSubset(displayString)) {
        return createPreposition(displayValue, u"uno ");
    }
    if (countValue == ItGrammarSynthesizer::Count::singular && genderValue == ItGrammarSynthesizer::Gender::masculine) {
        return createPreposition(displayValue, u"un ");
    }
    return createPreposition(displayValue, ::std::u16string());
}

} // namespace morphuntion::grammar::synthesis
