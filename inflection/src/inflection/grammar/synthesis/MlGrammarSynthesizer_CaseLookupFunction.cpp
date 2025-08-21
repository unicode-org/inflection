/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/
#include "MlGrammarSynthesizer_CaseLookupFunction.hpp"

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

// Constructor: initialize the member table here (no static locals).
MlGrammarSynthesizer_CaseLookupFunction::MlGrammarSynthesizer_CaseLookupFunction()
    : m_suffixToCase_{
        { u"ന്റെ", GrammemeConstants::CASE_GENITIVE() },
        { u"യുടെ", GrammemeConstants::CASE_GENITIVE() },
        { u"ഉടെ", GrammemeConstants::CASE_GENITIVE() },
        { u"ആയുടെ", GrammemeConstants::CASE_GENITIVE() },
        { u"ഉടേതു്", GrammemeConstants::CASE_GENITIVE() },
        { u"ഉടേതു", GrammemeConstants::CASE_GENITIVE() },
        { u"ഉടെത്", GrammemeConstants::CASE_GENITIVE() },
        { u"നെ", GrammemeConstants::CASE_ACCUSATIVE() },
        { u"ക്ക്", GrammemeConstants::CASE_DATIVE() },
        { u"യ്ക്ക്", GrammemeConstants::CASE_DATIVE() },
        { u"യിൽ", GrammemeConstants::CASE_LOCATIVE() },
        { u"ഇൽ", GrammemeConstants::CASE_LOCATIVE() },
        { u"ആൽ", GrammemeConstants::CASE_INSTRUMENTAL() },
        { u"വഴി", GrammemeConstants::CASE_INSTRUMENTAL() },
        { u"ഓടെ", GrammemeConstants::CASE_SOCIATIVE() }
    }
{
}

::inflection::dialog::SpeakableString* MlGrammarSynthesizer_CaseLookupFunction::getFeatureValue(
   const ::inflection::dialog::DisplayValue& displayValue,
   const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
   std::u16string displayString;
   ::inflection::util::StringViewUtils::lowercase(
       &displayString,
       displayValue.getDisplayString(),
       ::inflection::util::LocaleUtils::MALAYALAM());

   // Use std::u16string::ends_with with a u16string_view suffix
   for (const auto& pair : m_suffixToCase_) {
       const std::u16string_view suffix = pair.first;
       const std::u16string& caseGrammeme = pair.second;
       if (displayString.ends_with(suffix)) {
           return new ::inflection::dialog::SpeakableString(caseGrammeme);
       }
   }

   return nullptr;
}

} // namespace inflection::grammar::synthesis