/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/HeGrammarSynthesizer_WithConditionalHyphen.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>

namespace morphuntion::grammar::synthesis {

HeGrammarSynthesizer_WithConditionalHyphen::HeGrammarSynthesizer_WithConditionalHyphen()
    : super()
{
}

morphuntion::dialog::SpeakableString* HeGrammarSynthesizer_WithConditionalHyphen::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!::morphuntion::dictionary::PhraseProperties::isStartsWithUnicodeSets(displayString, ::morphuntion::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), ::morphuntion::lang::StringFilterUtil::HEBREW_SCRIPT(), {})) {
        return new ::morphuntion::dialog::SpeakableString(::morphuntion::dialog::SpeakableString(u"-") + ::morphuntion::dialog::SpeakableString(displayString));
    } else {
        return new ::morphuntion::dialog::SpeakableString(displayString);
    }
}

} // namespace morphuntion::grammar::synthesis
