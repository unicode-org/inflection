/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/EnCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticConceptList.hpp>

namespace inflection::dialog::language {

EnCommonConceptFactory::EnCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

EnCommonConceptFactory::~EnCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
