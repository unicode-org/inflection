/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/ZhCommonConceptFactory.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>

namespace morphuntion::dialog::language {

ZhCommonConceptFactory::ZhCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

morphuntion::dialog::SpeakableString ZhCommonConceptFactory::quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    return formattedNumber + morphuntion::dialog::SpeakableString(measureWord) + nounPhrase;
}

} // namespace morphuntion::dialog::language
