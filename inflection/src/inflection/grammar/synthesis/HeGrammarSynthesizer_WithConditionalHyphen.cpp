/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/HeGrammarSynthesizer_WithConditionalHyphen.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>

namespace inflection::grammar::synthesis {

HeGrammarSynthesizer_WithConditionalHyphen::HeGrammarSynthesizer_WithConditionalHyphen()
    : super()
{
}

inflection::dialog::SpeakableString* HeGrammarSynthesizer_WithConditionalHyphen::getFeatureValue(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    const auto& displayString = displayValue.getDisplayString();
    if (!::inflection::dictionary::PhraseProperties::isStartsWithUnicodeSets(displayString, ::inflection::dictionary::PhraseProperties::DEFAULT_MATCHABLE_SET(), ::inflection::lang::StringFilterUtil::HEBREW_SCRIPT(), {})) {
        return new ::inflection::dialog::SpeakableString(::inflection::dialog::SpeakableString(u"-") + ::inflection::dialog::SpeakableString(displayString));
    } else {
        return new ::inflection::dialog::SpeakableString(displayString);
    }
}

} // namespace inflection::grammar::synthesis
