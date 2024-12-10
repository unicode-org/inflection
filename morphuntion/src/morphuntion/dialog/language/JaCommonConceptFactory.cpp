/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/JaCommonConceptFactory.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

JaCommonConceptFactory::JaCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

JaCommonConceptFactory::~JaCommonConceptFactory()
{
}

morphuntion::dialog::SemanticConceptList* JaCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createOrList(concepts))->setAfterFirst(morphuntion::dialog::SpeakableString(u"または")))->setItemDelimiter(morphuntion::dialog::SpeakableString(u"、"))->setBeforeLast(morphuntion::dialog::SpeakableString(u"")));
}

morphuntion::dialog::SemanticConceptList* JaCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createAndList(concepts))->setAfterFirst(morphuntion::dialog::SpeakableString(u"と")))->setItemDelimiter(morphuntion::dialog::SpeakableString(u"、"))->setBeforeLast(morphuntion::dialog::SpeakableString(u"")));
}

morphuntion::dialog::SpeakableString JaCommonConceptFactory::quote(const ::morphuntion::dialog::SpeakableString& str) const
{
    if (str.speakEqualsPrint()) {
        return ::morphuntion::dialog::SpeakableString(u"“" + str.getPrint() + u"”");
    }
    return ::morphuntion::dialog::SpeakableString(u"“" + str.getPrint() + u"”", str.getSpeak());
}

morphuntion::dialog::SpeakableString JaCommonConceptFactory::quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    return formattedNumber + morphuntion::dialog::SpeakableString(measureWord) + nounPhrase;
}

} // namespace morphuntion::dialog::language
