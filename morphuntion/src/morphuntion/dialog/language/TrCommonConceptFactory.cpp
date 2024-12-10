/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/TrCommonConceptFactory.hpp>

#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

TrCommonConceptFactory::TrCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

TrCommonConceptFactory::~TrCommonConceptFactory()
{
}

morphuntion::dialog::SemanticConceptList* TrCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u" ya da "));
}

morphuntion::dialog::SemanticConceptList* TrCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createAndList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u" ve "));
}

} // namespace morphuntion::dialog::language
