/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/DaCommonConceptFactory.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::dialog::language {

DaCommonConceptFactory::DaCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_COMMON(), u"cardinal-common"},
        {::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

DaCommonConceptFactory::~DaCommonConceptFactory()
{
}

} // namespace inflection::dialog::language
