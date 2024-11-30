/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/YueCommonConceptFactory.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

YueCommonConceptFactory::YueCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

YueCommonConceptFactory::~YueCommonConceptFactory()
{
}

morphuntion::dialog::SemanticConceptList* YueCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createOrList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u"或")))->setItemDelimiter(morphuntion::dialog::SpeakableString(u"、")));
}

morphuntion::dialog::SemanticConceptList* YueCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createAndList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u"同")))->setItemDelimiter(morphuntion::dialog::SpeakableString(u"、")));
}

morphuntion::dialog::SpeakableString YueCommonConceptFactory::quote(const ::morphuntion::dialog::SpeakableString& str) const
{
    const auto& printStr = str.getPrint();
    if (::morphuntion::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(printStr)) {
        return super::quote(str);
    }
    if (str.speakEqualsPrint()) {
        return ::morphuntion::dialog::SpeakableString(u"“" + str.getPrint() + u"”");
    }
    return ::morphuntion::dialog::SpeakableString(u"“" + printStr + u"”", str.getSpeak());
}

morphuntion::dialog::SpeakableString YueCommonConceptFactory::quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    return formattedNumber + morphuntion::dialog::SpeakableString(measureWord) + nounPhrase;
}

} // namespace morphuntion::dialog::language
