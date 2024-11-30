/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/EnCommonConceptFactory.hpp>

#include <morphuntion/dialog/SemanticConceptList.hpp>

namespace morphuntion::dialog::language {

EnCommonConceptFactory::EnCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
}

EnCommonConceptFactory::~EnCommonConceptFactory()
{
}

} // namespace morphuntion::dialog::language
