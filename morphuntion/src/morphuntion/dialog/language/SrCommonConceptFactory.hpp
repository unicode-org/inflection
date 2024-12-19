/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::SrCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:

    explicit SrCommonConceptFactory(const ::morphuntion::util::ULocale& language);
    ~SrCommonConceptFactory() override;
};
