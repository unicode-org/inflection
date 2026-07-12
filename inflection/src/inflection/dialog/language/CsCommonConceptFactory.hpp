/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/language/SlavicCommonConceptFactory.hpp>

class inflection::dialog::language::CsCommonConceptFactory
    : public SlavicCommonConceptFactory
{
public:
    typedef SlavicCommonConceptFactory super;
    explicit CsCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~CsCommonConceptFactory() override;
};
