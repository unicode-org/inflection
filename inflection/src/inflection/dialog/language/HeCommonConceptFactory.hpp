/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <set>

class inflection::dialog::language::HeCommonConceptFactory
    : public inflection::dialog::CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

public: /* protected */
    ::std::set<::std::u16string_view> dontQuantify {  };
    const SemanticFeature& semanticFeatureGender;
    const SemanticFeature& semanticFeatureDefiniteness;

public:
    SemanticConceptList* createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const override;
    SemanticConceptList* createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const override;

private:
    bool isDefinite(const SemanticFeatureConceptBase* semanticConcept) const;

public:
    inflection::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

protected: /* package */
    ::inflection::dialog::SpeakableString quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& measureWord, Plurality::Rule countType) const override;

public:
    explicit HeCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~HeCommonConceptFactory() override;
};
