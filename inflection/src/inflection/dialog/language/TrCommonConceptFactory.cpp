/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/TrCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

TrCommonConceptFactory::TrCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

TrCommonConceptFactory::~TrCommonConceptFactory()
{
}

inflection::dialog::SemanticConceptList* TrCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u" ya da "));
}

inflection::dialog::SemanticConceptList* TrCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createAndList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u" ve "));
}

} // namespace inflection::dialog::language
