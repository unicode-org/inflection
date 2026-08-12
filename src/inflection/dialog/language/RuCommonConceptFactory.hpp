/*
 * Copyright 2019-2026 Apple Inc. and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/language/SlavicCommonConceptFactory.hpp>

class inflection::dialog::language::RuCommonConceptFactory
    : public SlavicCommonConceptFactory
{
public:
    typedef SlavicCommonConceptFactory super;
    explicit RuCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~RuCommonConceptFactory() override;
};
