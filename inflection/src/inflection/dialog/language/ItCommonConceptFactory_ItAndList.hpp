/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <icu4cxx/fwd.hpp>

class inflection::dialog::language::ItCommonConceptFactory_ItAndList final
    : public inflection::dialog::SemanticConceptList
{
public:
    typedef inflection::dialog::SemanticConceptList super;

public:
    ItCommonConceptFactory_ItAndList* clone() const override;
    SpeakableString getBeforeLast(const SemanticFeatureConceptBase& secondToLast, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& last, const SpeakableString& formattedLast) const override;


public:
    ItCommonConceptFactory_ItAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values);
    ItCommonConceptFactory_ItAndList(const ItCommonConceptFactory_ItAndList& other);
    ~ItCommonConceptFactory_ItAndList() override;

    static const icu4cxx::UnicodeSet& IMPORTANT_SET();

private:
    ItCommonConceptFactory_ItAndList& operator=(const ItCommonConceptFactory_ItAndList&) = delete;
};
