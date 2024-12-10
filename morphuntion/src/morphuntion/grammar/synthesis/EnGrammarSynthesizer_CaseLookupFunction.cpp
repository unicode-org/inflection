/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/grammar/synthesis/EnGrammarSynthesizer_CaseLookupFunction.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/dialog/SemanticFeature.hpp>
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/resources/DataResource.hpp>
#include <morphuntion/util/DelimitedStringIterator.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>

namespace morphuntion::grammar::synthesis {

static const char16_t EXPANSION_TABLE_NAME[] = {u"/org/unicode/morphuntion/contraction/contractionExpandingTable_en.csv"};

EnGrammarSynthesizer_CaseLookupFunction::EnGrammarSynthesizer_CaseLookupFunction()
    : super()
{
    auto reader = ::morphuntion::resources::DataResource::getString(EXPANSION_TABLE_NAME);
    for (::morphuntion::util::DelimitedStringIterator iterator(reader, u"\n"); iterator.hasNext(); ++iterator) {
        ::morphuntion::util::DelimitedStringIterator lineTokens(*iterator, u",");
        ::std::u16string currentCompoundToken;
        morphuntion::util::StringViewUtils::lowercase(&currentCompoundToken, *lineTokens, morphuntion::util::LocaleUtils::ROOT());
        if (::morphuntion::util::StringViewUtils::endsWith(currentCompoundToken, u"'s")) {
            contractions.insert(currentCompoundToken);
        }
    }
}

morphuntion::dialog::SpeakableString* EnGrammarSynthesizer_CaseLookupFunction::getFeatureValue(const ::morphuntion::dialog::SemanticFeatureModel_DisplayValue& displayValue, const ::std::map<::morphuntion::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    std::u16string displayString;
    ::morphuntion::util::StringViewUtils::lowercase(&displayString, displayValue.getDisplayString(), ::morphuntion::util::LocaleUtils::ENGLISH());
    if (displayString.length() >= 3) {
        std::u16string normalizedString;
        ::morphuntion::util::UnicodeSetUtils::replaceSetFromString(&normalizedString, morphuntion::dictionary::PhraseProperties::SINGLE_QUOTES(), displayString, u"'");
        // It's singular possessive, plural possessive or an edge case and not a known contraction.
        if ((::morphuntion::util::StringViewUtils::endsWith(normalizedString, u"'s") || ::morphuntion::util::StringViewUtils::endsWith(normalizedString, u"s'") || normalizedString == u"its")
            && contractions.find(normalizedString) == contractions.end())
        {
            return new ::morphuntion::dialog::SpeakableString(GrammemeConstants::CASE_GENITIVE());
        }
    }
    return nullptr;
}

} // namespace morphuntion::grammar::synthesis
