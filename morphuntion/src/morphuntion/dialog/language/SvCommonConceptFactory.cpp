/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/SvCommonConceptFactory.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::dialog::language {

SvCommonConceptFactory::SvCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language, ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER, {
        {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_COMMON(), u"cardinal-reale"},
        {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

SvCommonConceptFactory::~SvCommonConceptFactory()
{
}

} // namespace morphuntion::dialog::language
