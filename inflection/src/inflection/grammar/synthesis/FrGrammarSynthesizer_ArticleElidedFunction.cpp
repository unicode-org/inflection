/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/FrGrammarSynthesizer_ArticleElidedFunction.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SpeakableString.hpp>

namespace inflection::grammar::synthesis {

FrGrammarSynthesizer_ArticleElidedFunction::FrGrammarSynthesizer_ArticleElidedFunction(const ::inflection::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& startsWithConsonant, const ::std::u16string& startsWithVowel)
    : super(model, derivedSemanticName != nullptr, insertSpace)
    , defaultString(defaultString)
    , dontDoElison(startsWithConsonant)
    , doElision(startsWithVowel)
{
}

inflection::dialog::SpeakableString* FrGrammarSynthesizer_ArticleElidedFunction::getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!displayString.empty()) {
        if (::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::FRENCH(), displayString)) {
            return createPreposition(displayValue, doElision);
        }
        else {
            return createPreposition(displayValue, dontDoElison);
        }
    }
    return createPreposition(displayValue, defaultString);
}

} // namespace inflection::grammar::synthesis
