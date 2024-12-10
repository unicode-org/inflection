/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <set>

class morphuntion::dialog::language::HeCommonConceptFactory
    : public morphuntion::dialog::CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

public: /* protected */
    ::std::set<::std::u16string_view> dontQuantify {  };
    const SemanticFeature& semanticFeatureGender;
    const SemanticFeature& semanticFeatureDefiniteness;

public:
    SemanticConceptList* createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const override;

private:
    bool isDefinite(const SemanticFeatureConceptBase* semanticConcept) const;

public:
    morphuntion::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

protected: /* package */
    ::morphuntion::dialog::SpeakableString quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule countType) const override;

public:
    explicit HeCommonConceptFactory(const ::morphuntion::util::ULocale& language);
    ~HeCommonConceptFactory() override;
};
