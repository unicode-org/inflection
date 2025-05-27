/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */

#include <inflection/dialog/language/MlCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>

using inflection::grammar::synthesis::GrammemeConstants;

namespace inflection::dialog::language {

MlCommonConceptFactory::MlCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
}

MlCommonConceptFactory::~MlCommonConceptFactory()
{
}

} // namespace inflection::dialog::language

