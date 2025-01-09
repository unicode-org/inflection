/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/JaCommonConceptFactory.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

JaCommonConceptFactory::JaCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

JaCommonConceptFactory::~JaCommonConceptFactory()
{
}

inflection::dialog::SemanticConceptList* JaCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createOrList(concepts))->setAfterFirst(inflection::dialog::SpeakableString(u"または")))->setItemDelimiter(inflection::dialog::SpeakableString(u"、"))->setBeforeLast(inflection::dialog::SpeakableString(u"")));
}

inflection::dialog::SemanticConceptList* JaCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createAndList(concepts))->setAfterFirst(inflection::dialog::SpeakableString(u"と")))->setItemDelimiter(inflection::dialog::SpeakableString(u"、"))->setBeforeLast(inflection::dialog::SpeakableString(u"")));
}

inflection::dialog::SpeakableString JaCommonConceptFactory::quote(const ::inflection::dialog::SpeakableString& str) const
{
    if (str.speakEqualsPrint()) {
        return ::inflection::dialog::SpeakableString(u"“" + str.getPrint() + u"”");
    }
    return ::inflection::dialog::SpeakableString(u"“" + str.getPrint() + u"”", str.getSpeak());
}

inflection::dialog::SpeakableString JaCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    return formattedNumber + inflection::dialog::SpeakableString(measureWord) + nounPhrase;
}

} // namespace inflection::dialog::language
