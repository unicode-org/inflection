/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/language/SlavicCommonConceptFactory.hpp>

class inflection::dialog::language::UkCommonConceptFactory
    : public SlavicCommonConceptFactory
{
public:
    typedef SlavicCommonConceptFactory super;
    explicit UkCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~UkCommonConceptFactory() override;
};
