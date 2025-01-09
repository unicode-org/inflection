/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/NlCommonConceptFactory.hpp>

namespace inflection::dialog::language {

NlCommonConceptFactory::NlCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

NlCommonConceptFactory::~NlCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
