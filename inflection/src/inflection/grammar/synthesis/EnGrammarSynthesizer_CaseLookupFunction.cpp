/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include <inflection/grammar/synthesis/EnGrammarSynthesizer_CaseLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/util/DelimitedStringIterator.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>

namespace inflection::grammar::synthesis {

static const char16_t EXPANSION_TABLE_NAME[] = {u"/org/unicode/inflection/contraction/contractionExpandingTable_en.csv"};

EnGrammarSynthesizer_CaseLookupFunction::EnGrammarSynthesizer_CaseLookupFunction()
    : super()
{
    auto reader = ::inflection::resources::DataResource::getString(EXPANSION_TABLE_NAME);
    for (::inflection::util::DelimitedStringIterator iterator(reader, u"\n"); iterator.hasNext(); ++iterator) {
        ::inflection::util::DelimitedStringIterator lineTokens(*iterator, u",");
        ::std::u16string currentCompoundToken;
        inflection::util::StringViewUtils::lowercase(&currentCompoundToken, *lineTokens, inflection::util::LocaleUtils::ROOT());
        if (currentCompoundToken.ends_with(u"'s")) {
            contractions.insert(currentCompoundToken);
        }
    }
}

inflection::dialog::SpeakableString* EnGrammarSynthesizer_CaseLookupFunction::getFeatureValue(const ::inflection::dialog::DisplayValue& displayValue, const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
    std::u16string displayString;
    ::inflection::util::StringViewUtils::lowercase(&displayString, displayValue.getDisplayString(), ::inflection::util::LocaleUtils::ENGLISH());
    if (displayString.length() >= 3) {
        std::u16string normalizedString;
        ::inflection::util::UnicodeSetUtils::replaceSetFromString(&normalizedString, inflection::dictionary::PhraseProperties::SINGLE_QUOTES(), displayString, u"'");
        // It's singular possessive, plural possessive or an edge case and not a known contraction.
        if ((normalizedString.ends_with(u"'s") || normalizedString.ends_with(u"s'") || normalizedString == u"its")
            && contractions.find(normalizedString) == contractions.end())
        {
            return new ::inflection::dialog::SpeakableString(GrammemeConstants::CASE_GENITIVE());
        }
    }
    return nullptr;
}

} // namespace inflection::grammar::synthesis
