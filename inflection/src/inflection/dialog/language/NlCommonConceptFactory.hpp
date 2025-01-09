/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::NlCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:
public:
    explicit NlCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~NlCommonConceptFactory() override;
};
