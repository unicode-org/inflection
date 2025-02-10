/*
 * Copyright 2024 and later: Unicode, Inc. and others.
 * License & terms of use: http://www.unicode.org/copyright.html
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::SrCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

private:
    const SemanticFeature &semanticFeatureCase;
    const SemanticFeature &semanticFeatureGender;
    const SemanticFeature &semanticFeatureNumber;
    const SemanticFeature &semanticFeatureAnimacy;

public:
    explicit SrCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~SrCommonConceptFactory() override;
};
