/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */

#include <inflection/dialog/language/MlCommonConceptFactory.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/dialog/Plurality.hpp>

namespace inflection::dialog::language {

// Malayalam-specific conjunction for OR
::inflection::dialog::SemanticConceptList* MlCommonConceptFactory::createOrList(
    const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    auto list = super::createOrList(concepts);
    if (list) {
        list->setBeforeLast(::inflection::dialog::SpeakableString(u" അല്ലെങ്കിൽ "));
    }
    return list;
}

// Malayalam-specific conjunction for AND
::inflection::dialog::SemanticConceptList* MlCommonConceptFactory::createAndList(
    const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    auto list = super::createAndList(concepts);
    if (list) {
        list->setBeforeLast(::inflection::dialog::SpeakableString(u"യും "));
        list->setItemDelimiter(::inflection::dialog::SpeakableString(u", "));
    }
    return list;
}

// In Malayalam, numbers generally follow the noun
::inflection::dialog::SpeakableString
MlCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber,
                                       const ::inflection::dialog::SpeakableString& nounPhrase,
                                       const ::std::u16string& /*measureWord*/,
                                       Plurality::Rule countType) const
{
    ::inflection::dialog::SpeakableString space(u" ");
    if (countType == Plurality::Rule::ONE) {
        return nounPhrase + space + formattedNumber;
    }
    return formattedNumber + space + nounPhrase;
}

// Fallback to base implementation for now
::inflection::dialog::SpeakableString
MlCommonConceptFactory::quantifyType(const ::inflection::dialog::SpeakableString& formattedNumber,
                                     const ::inflection::dialog::SemanticFeatureConceptBase& semanticConcept,
                                     bool useDefault,
                                     ::inflection::dialog::Plurality::Rule countType) const
{
    return super::quantifyType(formattedNumber, semanticConcept, useDefault, countType);
}

} // namespace inflection::dialog::language
