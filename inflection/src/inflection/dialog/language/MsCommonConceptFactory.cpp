/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/MsCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

MsCommonConceptFactory::MsCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

MsCommonConceptFactory::~MsCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
