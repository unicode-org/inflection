/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/KoCommonConceptFactory.hpp>

#include <inflection/dialog/language/KoCommonConceptFactory_KoAndList.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

KoCommonConceptFactory::KoCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

inflection::dialog::SemanticConceptList* KoCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u"또는 "));
}

inflection::dialog::SemanticConceptList* KoCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new KoCommonConceptFactory_KoAndList(getSemanticFeatureModel(), concepts);
}

inflection::dialog::SpeakableString KoCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    ::inflection::dialog::SpeakableString phrase(formattedNumber);
    auto noWhiteSpaces = ::inflection::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(nounPhrase.getPrint());
    if (!noWhiteSpaces) {
        phrase = phrase + ::inflection::dialog::SpeakableString(u" ");
    }
    phrase = phrase + nounPhrase;
    if (!measureWord.empty()) {
        if (!noWhiteSpaces) {
            phrase = phrase + ::inflection::dialog::SpeakableString(u" ");
        }
        phrase = phrase + ::inflection::dialog::SpeakableString(measureWord);
    }
    return phrase;
}

} // namespace inflection::dialog::language
