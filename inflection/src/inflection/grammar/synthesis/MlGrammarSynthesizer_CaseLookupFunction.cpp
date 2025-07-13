/*
* Copyright 2025 Unicode Incorporated and others. All rights reserved.
*/
#include <inflection/grammar/synthesis/MlGrammarSynthesizer_CaseLookupFunction.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/dialog/DisplayValue.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::grammar::synthesis {

bool ends_with(const std::u16string& str, const std::u16string& suffix) {
   if (suffix.size() >= str.size()) return false;
   return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

inflection::dialog::SpeakableString* MlGrammarSynthesizer_CaseLookupFunction::getFeatureValue(
   const ::inflection::dialog::DisplayValue& displayValue,
   const ::std::map<::inflection::dialog::SemanticFeature, ::std::u16string>& /*constraints*/) const
{
   std::u16string displayString;
   ::inflection::util::StringViewUtils::lowercase(&displayString, displayValue.getDisplayString(), ::inflection::util::LocaleUtils::MALAYALAM());

   static const std::vector<std::pair<std::u16string, std::u16string>> suffixToCase = {
        {u"ന്റെ", GrammemeConstants::CASE_GENITIVE()},
        {u"യുടെ", GrammemeConstants::CASE_GENITIVE()},
        {u"ഉടെ", GrammemeConstants::CASE_GENITIVE()},
        {u"ആയുടെ", GrammemeConstants::CASE_GENITIVE()},
        {u"ഉടേതു്", GrammemeConstants::CASE_GENITIVE()},
        {u"ഉടേതു", GrammemeConstants::CASE_GENITIVE()},
        {u"ഉടെത്", GrammemeConstants::CASE_GENITIVE()},
        {u"നെ", GrammemeConstants::CASE_ACCUSATIVE()},
        {u"ക്ക്", GrammemeConstants::CASE_DATIVE()},
        {u"യ്ക്ക്", GrammemeConstants::CASE_DATIVE()},
        {u"യിൽ", GrammemeConstants::CASE_LOCATIVE()},
        {u"ഇൽ", GrammemeConstants::CASE_LOCATIVE()},
        {u"ആൽ", GrammemeConstants::CASE_INSTRUMENTAL()},
        {u"വഴി", GrammemeConstants::CASE_INSTRUMENTAL()},
        {u"ഓടെ", GrammemeConstants::CASE_SOCIATIVE()},
   };

   for (const auto& [suffix, caseGrammeme] : suffixToCase) {
       if (ends_with(displayString, suffix)) {
           return new ::inflection::dialog::SpeakableString(caseGrammeme);
       }
   }

   return nullptr;
}

MlGrammarSynthesizer_CaseLookupFunction::MlGrammarSynthesizer_CaseLookupFunction() = default;

} // namespace inflection::grammar::synthesis

