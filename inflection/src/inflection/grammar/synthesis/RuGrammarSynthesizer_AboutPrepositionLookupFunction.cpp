/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/RuGrammarSynthesizer_AboutPrepositionLookupFunction.hpp>

#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>

namespace inflection::grammar::synthesis {


RuGrammarSynthesizer_AboutPrepositionLookupFunction::RuGrammarSynthesizer_AboutPrepositionLookupFunction(const ::inflection::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

inflection::dialog::SpeakableString* RuGrammarSynthesizer_AboutPrepositionLookupFunction::getArticle(const ::inflection::dialog::SemanticFeatureModel_DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::inflection::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::inflection::util::LocaleUtils::RUSSIAN());
    if (::inflection::util::StringViewUtils::startsWith(displayStringLowerCase, u"вс") || ::inflection::util::StringViewUtils::startsWith(displayStringLowerCase, u"рт") || ::inflection::util::StringViewUtils::startsWith(displayStringLowerCase, u"мн")) {
        return createPreposition(displayValue, u"обо");
    }
    if (::inflection::dictionary::PhraseProperties::isStartsWithVowel(::inflection::util::LocaleUtils::RUSSIAN(), displayString)) {
        return createPreposition(displayValue, u"об");
    }
    return createPreposition(displayValue, u"о");
}

} // namespace inflection::grammar::synthesis
