/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/dialog/language/UkCommonConceptFactory.hpp>

namespace inflection::dialog::language {

UkCommonConceptFactory::UkCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, {
        /* few */            Agreement::PAUCAL_PLURAL,
        /* many */           Agreement::GOVERNED_PLURAL,
        /* other */          Agreement::FRACTION_GENITIVE_SG,
        /* animateAccusativeNumeral */ true,
    })
{
}

UkCommonConceptFactory::~UkCommonConceptFactory() = default;

} // namespace inflection::dialog::language
