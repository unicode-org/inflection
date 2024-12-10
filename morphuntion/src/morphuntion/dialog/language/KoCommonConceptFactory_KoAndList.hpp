/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>

class morphuntion::dialog::language::KoCommonConceptFactory_KoAndList final
    : public morphuntion::dialog::SemanticConceptList
{
public:
    typedef morphuntion::dialog::SemanticConceptList super;

public:
    KoCommonConceptFactory_KoAndList* clone() const override;
    SpeakableString getBeforeLast(const SemanticFeatureConceptBase& secondToLast, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& last, const SpeakableString& formattedLast) const override;


public:
    KoCommonConceptFactory_KoAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values);
    KoCommonConceptFactory_KoAndList(const KoCommonConceptFactory_KoAndList& other);
    ~KoCommonConceptFactory_KoAndList() override;

private:
    KoCommonConceptFactory_KoAndList& operator=(const KoCommonConceptFactory_KoAndList&) = delete;
};
