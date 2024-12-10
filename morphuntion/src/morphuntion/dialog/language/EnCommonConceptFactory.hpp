/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::EnCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:
public:
    explicit EnCommonConceptFactory(const ::morphuntion::util::ULocale& language);
    ~EnCommonConceptFactory() override;
};
