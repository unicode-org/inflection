/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::FrCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:
    ::morphuntion::dialog::SpeakableString quote(const ::morphuntion::dialog::SpeakableString& str) const override;

    explicit FrCommonConceptFactory(const ::morphuntion::util::ULocale& language);
    ~FrCommonConceptFactory() override;
};
