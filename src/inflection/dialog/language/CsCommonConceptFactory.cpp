/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/dialog/language/CsCommonConceptFactory.hpp>

namespace inflection::dialog::language {

CsCommonConceptFactory::CsCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, {
        /* few */            Agreement::PAUCAL_PLURAL,
        /* many */           Agreement::FRACTION_GENITIVE_SG,
        /* other */          Agreement::GOVERNED_PLURAL,
        /* animateAccusativeNumeral */ true,
    })
{
}

CsCommonConceptFactory::~CsCommonConceptFactory() = default;

} // namespace inflection::dialog::language
