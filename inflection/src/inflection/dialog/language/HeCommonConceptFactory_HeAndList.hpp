/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>

class inflection::dialog::language::HeCommonConceptFactory_HeAndList final
    : public inflection::dialog::SemanticConceptList
{
public:
    typedef inflection::dialog::SemanticConceptList super;

public:
    HeCommonConceptFactory_HeAndList* clone() const override;
    SpeakableString getBeforeLast(const SemanticFeatureConceptBase& secondToLast, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& last, const SpeakableString& formattedLast) const override;

public:
    HeCommonConceptFactory_HeAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values);
    HeCommonConceptFactory_HeAndList(const HeCommonConceptFactory_HeAndList& other);
    ~HeCommonConceptFactory_HeAndList() override;

private:
    HeCommonConceptFactory_HeAndList& operator=(const HeCommonConceptFactory_HeAndList&) = delete;
};
