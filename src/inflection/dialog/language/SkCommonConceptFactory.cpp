/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/dialog/language/SkCommonConceptFactory.hpp>

namespace inflection::dialog::language {

SkCommonConceptFactory::SkCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, {
        /* few */            Agreement::PAUCAL_PLURAL,
        /* many */           Agreement::FRACTION_GENITIVE_SG,
        /* other */          Agreement::GOVERNED_PLURAL,
        /* animateAccusativeNumeral */ true,
    })
{
}

SkCommonConceptFactory::~SkCommonConceptFactory() = default;

} // namespace inflection::dialog::language
