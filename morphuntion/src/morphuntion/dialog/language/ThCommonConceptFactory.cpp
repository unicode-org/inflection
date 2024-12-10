/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/ThCommonConceptFactory.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

ThCommonConceptFactory::ThCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

morphuntion::dialog::SpeakableString ThCommonConceptFactory::quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    morphuntion::dialog::SpeakableString phrase(nounPhrase + ::morphuntion::dialog::SpeakableString(u" ") + formattedNumber);
    if (!measureWord.empty()) {
        phrase += morphuntion::dialog::SpeakableString(u" ") + morphuntion::dialog::SpeakableString(measureWord);
    }
    return phrase;
}

morphuntion::dialog::SemanticConceptList* ThCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setItemDelimiter(SpeakableString(u", "))->setBeforeLast(morphuntion::dialog::SpeakableString(u" หรือ "));
}

morphuntion::dialog::SemanticConceptList* ThCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createAndList(concepts))->setItemDelimiter(SpeakableString(u", "))->setBeforeLast(morphuntion::dialog::SpeakableString(u" และ "));
}

} // namespace morphuntion::dialog::language
