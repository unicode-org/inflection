/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */

#include <inflection/dialog/language/MlCommonConceptFactory.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/Plurality.hpp>

namespace inflection::dialog::language {

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

} // namespace inflection::dialog::language
