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

   static const std::vector<std::u16string> suffixes = {
       u"ഉടെ",   
       u"യുടെ",   
       u"ന്റെ",  
       u"ആയുടെ", 
       u"ഉടേതു്",
       u"ഉടേതു",
       u"ഉടെത്"
   };

   for (const auto& suffix : suffixes) {
       if (displayString.size() > suffix.size() && ends_with(displayString, suffix)) {
           return new ::inflection::dialog::SpeakableString(GrammemeConstants::CASE_GENITIVE());
       }
   }

   return nullptr;
}

MlGrammarSynthesizer_CaseLookupFunction::MlGrammarSynthesizer_CaseLookupFunction() = default;

} // namespace inflection::grammar::synthesis

