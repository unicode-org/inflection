/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/KoCommonConceptFactory_KoAndList.hpp>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dictionary/PhraseProperties.hpp>
#include <morphuntion/util/LocaleUtils.hpp>

namespace morphuntion::dialog::language {

KoCommonConceptFactory_KoAndList::KoCommonConceptFactory_KoAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(morphuntion::dialog::SpeakableString(u", "));
}

KoCommonConceptFactory_KoAndList::KoCommonConceptFactory_KoAndList(const KoCommonConceptFactory_KoAndList& other)
    : super(other)
{
}

KoCommonConceptFactory_KoAndList::~KoCommonConceptFactory_KoAndList()
{
}

KoCommonConceptFactory_KoAndList* KoCommonConceptFactory_KoAndList::clone() const
{
    return new KoCommonConceptFactory_KoAndList(*this);
}

morphuntion::dialog::SpeakableString KoCommonConceptFactory_KoAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& /*formattedLast*/) const
{
    if (::morphuntion::dictionary::PhraseProperties::isEndsWithVowel(::morphuntion::util::LocaleUtils::KOREAN(), formattedSecondToLast.getPrint())) {
        return morphuntion::dialog::SpeakableString(u"와 ");
    }
    return morphuntion::dialog::SpeakableString(u"과 ");
}

} // namespace morphuntion::dialog::language
