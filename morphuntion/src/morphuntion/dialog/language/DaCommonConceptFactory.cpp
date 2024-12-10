/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/DaCommonConceptFactory.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>

namespace morphuntion::dialog::language {

DaCommonConceptFactory::DaCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language, ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER, {
        {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_COMMON(), u"cardinal-common"},
        {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER(), u"cardinal-neuter"}})
{
}

DaCommonConceptFactory::~DaCommonConceptFactory()
{
}

} // namespace morphuntion::dialog::language
