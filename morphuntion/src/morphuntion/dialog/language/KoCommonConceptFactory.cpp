/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/KoCommonConceptFactory.hpp>

#include <morphuntion/dialog/language/KoCommonConceptFactory_KoAndList.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

KoCommonConceptFactory::KoCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

morphuntion::dialog::SemanticConceptList* KoCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u"또는 "));
}

morphuntion::dialog::SemanticConceptList* KoCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new KoCommonConceptFactory_KoAndList(getSemanticFeatureModel(), concepts);
}

morphuntion::dialog::SpeakableString KoCommonConceptFactory::quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule /*countType*/) const
{
    ::morphuntion::dialog::SpeakableString phrase(formattedNumber);
    auto noWhiteSpaces = ::morphuntion::lang::StringFilterUtil::isFirstNonWhitespaceSeparableCharacter(nounPhrase.getPrint());
    if (!noWhiteSpaces) {
        phrase = phrase + ::morphuntion::dialog::SpeakableString(u" ");
    }
    phrase = phrase + nounPhrase;
    if (!measureWord.empty()) {
        if (!noWhiteSpaces) {
            phrase = phrase + ::morphuntion::dialog::SpeakableString(u" ");
        }
        phrase = phrase + ::morphuntion::dialog::SpeakableString(measureWord);
    }
    return phrase;
}

} // namespace morphuntion::dialog::language
