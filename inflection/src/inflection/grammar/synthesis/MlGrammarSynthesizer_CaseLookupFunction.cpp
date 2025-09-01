/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#include "MlGrammarSynthesizer_CaseLookupFunction.hpp"

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>

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
   const std::u16string& displayString(displayValue.getDisplayString());

   for (const auto& [suffix, caseGrammeme] : m_suffixToCase_) {
       if (displayString.ends_with(suffix)) {
           return new ::inflection::dialog::SpeakableString(caseGrammeme);
       }
   }

   return nullptr;
}

} // namespace inflection::grammar::synthesis