/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>

class inflection::dialog::language::EsCommonConceptFactory_EsOrList final
    : public inflection::dialog::SemanticConceptList
{
public:
    typedef inflection::dialog::SemanticConceptList super;

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
