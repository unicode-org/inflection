/*
 * Copyright 2025 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::MlCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:
    explicit MlCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~MlCommonConceptFactory() override;
};
