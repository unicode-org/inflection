/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::ArCommonConceptFactory
    : public morphuntion::dialog::CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

private:
    const SemanticFeature& semanticFeatureCase;

public:
    SemanticConceptList* createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;

protected: /* package */
    ::morphuntion::dialog::SpeakableString quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule countType) const override;
    ::morphuntion::dialog::SpeakableString quantifyType(const ::morphuntion::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const override;

public:
    explicit ArCommonConceptFactory(const ::morphuntion::util::ULocale& language);
};
