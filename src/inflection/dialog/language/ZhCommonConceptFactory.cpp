/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/ZhCommonConceptFactory.hpp>

#include <inflection/dialog/SpeakableString.hpp>

namespace inflection::dialog::language {

ZhCommonConceptFactory::ZhCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

inflection::dialog::SpeakableString ZhCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    return formattedNumber + inflection::dialog::SpeakableString(measureWord) + nounPhrase;
}

} // namespace inflection::dialog::language
