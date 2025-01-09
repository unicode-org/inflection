/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/SvCommonConceptFactory.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::dialog::language {

SvCommonConceptFactory::SvCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_COMMON(), u"cardinal-reale"},
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

SvCommonConceptFactory::~SvCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
