/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::EsCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

public:
    SemanticConceptList* createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const override;

public:
    explicit EsCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~EsCommonConceptFactory() override;
};