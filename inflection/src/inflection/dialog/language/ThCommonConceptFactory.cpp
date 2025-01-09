/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/ThCommonConceptFactory.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

ThCommonConceptFactory::ThCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

inflection::dialog::SpeakableString ThCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    inflection::dialog::SpeakableString phrase(nounPhrase + ::inflection::dialog::SpeakableString(u" ") + formattedNumber);
    if (!measureWord.empty()) {
        phrase += inflection::dialog::SpeakableString(u" ") + inflection::dialog::SpeakableString(measureWord);
    }
    return phrase;
}

inflection::dialog::SemanticConceptList* ThCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setItemDelimiter(SpeakableString(u", "))->setBeforeLast(inflection::dialog::SpeakableString(u" หรือ "));
}

inflection::dialog::SemanticConceptList* ThCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createAndList(concepts))->setItemDelimiter(SpeakableString(u", "))->setBeforeLast(inflection::dialog::SpeakableString(u" และ "));
}

} // namespace inflection::dialog::language
