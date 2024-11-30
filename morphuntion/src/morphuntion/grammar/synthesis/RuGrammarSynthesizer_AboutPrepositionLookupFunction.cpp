/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/RuGrammarSynthesizer_AboutPrepositionLookupFunction.hpp>

#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>

namespace morphuntion::grammar::synthesis {


RuGrammarSynthesizer_AboutPrepositionLookupFunction::RuGrammarSynthesizer_AboutPrepositionLookupFunction(const ::morphuntion::dialog::SemanticFeatureModel& model, const ::std::u16string& derivedSemanticName)
    : super(model, derivedSemanticName)
{
}

morphuntion::dialog::SpeakableString* RuGrammarSynthesizer_AboutPrepositionLookupFunction::getArticle(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue) const
{
    const auto& displayString = displayValue.getDisplayString();
    ::std::u16string displayStringLowerCase;
    ::morphuntion::util::StringViewUtils::lowercase(&displayStringLowerCase, displayString, ::morphuntion::util::LocaleUtils::RUSSIAN());
    if (::morphuntion::util::StringViewUtils::startsWith(displayStringLowerCase, u"вс") || ::morphuntion::util::StringViewUtils::startsWith(displayStringLowerCase, u"рт") || ::morphuntion::util::StringViewUtils::startsWith(displayStringLowerCase, u"мн")) {
        return createPreposition(displayValue, u"обо");
    }
    if (::morphuntion::dictionary::PhraseProperties::isStartsWithVowel(::morphuntion::util::LocaleUtils::RUSSIAN(), displayString)) {
        return createPreposition(displayValue, u"об");
    }
    return createPreposition(displayValue, u"о");
}

} // namespace morphuntion::grammar::synthesis
