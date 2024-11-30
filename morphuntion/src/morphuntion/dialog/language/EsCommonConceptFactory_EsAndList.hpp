/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <icu4cxx/fwd.hpp>

class morphuntion::dialog::language::EsCommonConceptFactory_EsAndList final
    : public morphuntion::dialog::SemanticConceptList
{

public:
    typedef morphuntion::dialog::SemanticConceptList super;

public:
    EsCommonConceptFactory_EsAndList* clone() const override;
    SpeakableString getBeforeLast(const SemanticFeatureConceptBase& secondToLast, const SpeakableString& formattedSecondToLast, const SemanticFeatureConceptBase& last, const SpeakableString& formattedLast) const override;

public:
    EsCommonConceptFactory_EsAndList(const SemanticFeatureModel* model, const ::std::vector<const SemanticFeatureConceptBase*>& values);
    EsCommonConceptFactory_EsAndList(const EsCommonConceptFactory_EsAndList& other);
    ~EsCommonConceptFactory_EsAndList() override;

    static const icu4cxx::UnicodeSet& IMPORTANT_SET();
    static const icu4cxx::UnicodeSet& NON_SPANISH_VOWEL_SET();

private:
    EsCommonConceptFactory_EsAndList& operator=(const EsCommonConceptFactory_EsAndList&) = delete;
};
