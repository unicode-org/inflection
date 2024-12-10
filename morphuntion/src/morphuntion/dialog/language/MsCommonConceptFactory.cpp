/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/MsCommonConceptFactory.hpp>

#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

MsCommonConceptFactory::MsCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

MsCommonConceptFactory::~MsCommonConceptFactory()
{
}

} // namespace morphuntion::dialog::language
