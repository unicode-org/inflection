/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>

class morphuntion::dialog::language::ItCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

private:
    const SemanticFeature* semanticFeatureIndefiniteArticle {  };

public:
    SemanticConceptList* createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    morphuntion::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

public: /* package */
    ::morphuntion::dialog::SpeakableString quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule countType) const override;

public:
    explicit ItCommonConceptFactory(const ::morphuntion::util::ULocale& language);
    ~ItCommonConceptFactory() override;
};
