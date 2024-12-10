/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::EsCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

public:
    SemanticConceptList* createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;

public:
    explicit EsCommonConceptFactory(const ::morphuntion::util::ULocale& language);
    ~EsCommonConceptFactory() override;
};
