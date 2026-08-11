/*
 * Copyright 2019-2026 Apple Inc. and others. All rights reserved.
 */
#include <inflection/dialog/language/RuCommonConceptFactory.hpp>

namespace inflection::dialog::language {

RuCommonConceptFactory::RuCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, {
        /* few */            Agreement::PAUCAL_GOVERNED,
        /* many */           Agreement::GOVERNED_PLURAL,
        /* other */          Agreement::FRACTION_GENITIVE_SG,
        /* animateAccusativeNumeral */ false,
    })
{
}

RuCommonConceptFactory::~RuCommonConceptFactory() = default;

} // namespace inflection::dialog::language
