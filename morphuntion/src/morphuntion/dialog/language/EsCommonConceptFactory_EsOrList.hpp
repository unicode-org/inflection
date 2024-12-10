/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>

class morphuntion::dialog::language::EsCommonConceptFactory_EsOrList final
    : public morphuntion::dialog::SemanticConceptList
{
public:
    typedef morphuntion::dialog::SemanticConceptList super;

public:
    EsCommonConceptFactory_EsOrList* clone() const override;
    SpeakableString getBeforeLast(const SemanticFeatureConceptBase& secondToLast, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& last, const SpeakableString& formattedLast) const override;


public:
    EsCommonConceptFactory_EsOrList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values);
    EsCommonConceptFactory_EsOrList(const EsCommonConceptFactory_EsOrList& other);
    ~EsCommonConceptFactory_EsOrList() override;

private:
    EsCommonConceptFactory_EsOrList& operator=(const EsCommonConceptFactory_EsOrList&) = delete;
};
