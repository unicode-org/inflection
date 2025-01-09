/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/KoCommonConceptFactory_KoAndList.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dictionary/PhraseProperties.hpp>
#include <inflection/util/LocaleUtils.hpp>

namespace inflection::dialog::language {

KoCommonConceptFactory_KoAndList::KoCommonConceptFactory_KoAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values)
    : super(model, values)
{
    setItemDelimiter(inflection::dialog::SpeakableString(u", "));
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

inflection::dialog::SpeakableString KoCommonConceptFactory_KoAndList::getBeforeLast(const SemanticFeatureConceptBase& /*secondToLast*/, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& /*last*/, const SpeakableString& /*formattedLast*/) const
{
    if (::inflection::dictionary::PhraseProperties::isEndsWithVowel(::inflection::util::LocaleUtils::KOREAN(), formattedSecondToLast.getPrint())) {
        return inflection::dialog::SpeakableString(u"와 ");
    }
    return inflection::dialog::SpeakableString(u"과 ");
}

} // namespace inflection::dialog::language
