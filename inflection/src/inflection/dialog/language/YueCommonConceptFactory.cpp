/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/YueCommonConceptFactory.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

YueCommonConceptFactory::YueCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

YueCommonConceptFactory::~YueCommonConceptFactory()
{
}

inflection::dialog::SemanticConceptList* YueCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createOrList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u"或")))->setItemDelimiter(inflection::dialog::SpeakableString(u"、")));
}

inflection::dialog::SemanticConceptList* YueCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(npc(super::createAndList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u"同")))->setItemDelimiter(inflection::dialog::SpeakableString(u"、")));
}

inflection::dialog::SpeakableString YueCommonConceptFactory::quote(const ::inflection::dialog::SpeakableString& str) const
{
    const auto& printStr = str.getPrint();
    if (::inflection::lang::StringFilterUtil::hasNonWhitespaceSeparableCharacter(printStr)) {
        return super::quote(str);
    }
    if (str.speakEqualsPrint()) {
        return ::inflection::dialog::SpeakableString(u"“" + str.getPrint() + u"”");
    }
    return ::inflection::dialog::SpeakableString(u"“" + printStr + u"”", str.getSpeak());
}

inflection::dialog::SpeakableString YueCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    return formattedNumber + inflection::dialog::SpeakableString(measureWord) + nounPhrase;
}

} // namespace inflection::dialog::language
