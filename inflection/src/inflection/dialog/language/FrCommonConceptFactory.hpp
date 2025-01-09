/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::FrCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
public:
    ::inflection::dialog::SpeakableString quote(const ::inflection::dialog::SpeakableString& str) const override;

    explicit FrCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~FrCommonConceptFactory() override;
};
