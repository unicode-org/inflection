/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/FrGrammarSynthesizer_ArticleElidedFunction.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>

namespace morphuntion::grammar::synthesis {

FrGrammarSynthesizer_ArticleElidedFunction::FrGrammarSynthesizer_ArticleElidedFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, bool insertSpace, const char16_t* derivedSemanticName, const ::std::u16string& defaultString, const ::std::u16string& startsWithConsonant, const ::std::u16string& startsWithVowel)
    : super(model, derivedSemanticName != nullptr, insertSpace)
    , defaultString(defaultString)
    , dontDoElison(startsWithConsonant)
    , doElision(startsWithVowel)
{
}

morphuntion::dialog::SpeakableString* FrGrammarSynthesizer_ArticleElidedFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!displayString.empty()) {
        if (::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::FRENCH(), displayString)) {
            return createPreposition(displayValue, doElision);
        }
        else {
            return createPreposition(displayValue, dontDoElison);
        }
    }
    return createPreposition(displayValue, defaultString);
}

} // namespace morphuntion::grammar::synthesis
