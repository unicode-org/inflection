/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::NbCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:

public:
    explicit NbCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~NbCommonConceptFactory() override;
};
