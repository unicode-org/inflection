/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/ItGrammarSynthesizer_IndefiniteArticleLookupFunction.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/grammar/synthesis/ItGrammarSynthesizer.hpp>

namespace inflection::grammar::synthesis {


ItGrammarSynthesizer_IndefiniteArticleLookupFunction::ItGrammarSynthesizer_IndefiniteArticleLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

inflection::dialog::SpeakableString* ItGrammarSynthesizer_IndefiniteArticleLookupFunction::getArticle(const ::inflection::dialog::DisplayValue& displayValue, bool /*wantArticle*/, ItGrammarSynthesizer::Number numberValue, ItGrammarSynthesizer::Gender genderValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (genderValue == ItGrammarSynthesizer::Gender::feminine) {
        if (numberValue == ItGrammarSynthesizer::Number::plural) {
            return createPreposition(displayValue, u"delle ");
        }
        if (::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ITALIAN(), displayString)) {
            return createPreposition(displayValue, u"un’");
        }
        return createPreposition(displayValue, u"una ");
    }
    if (numberValue == ItGrammarSynthesizer::Number::plural) {
        if (::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::ITALIAN(), displayString) || ItGrammarSynthesizer::startsWithConsonantSubset(displayString)) {
            return createPreposition(displayValue, u"degli ");
        }
        return createPreposition(displayValue, u"dei ");
    }
    if (ItGrammarSynthesizer::startsWithConsonantSubset(displayString)) {
        return createPreposition(displayValue, u"uno ");
    }
    if (numberValue == ItGrammarSynthesizer::Number::singular && genderValue == ItGrammarSynthesizer::Gender::masculine) {
        return createPreposition(displayValue, u"un ");
    }
    return createPreposition(displayValue, ::std::u16string());
}

} // namespace inflection::grammar::synthesis
