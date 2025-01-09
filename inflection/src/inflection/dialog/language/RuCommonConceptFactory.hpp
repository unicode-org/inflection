/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::RuCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

private:
    const SemanticFeature& semanticFeatureCase;
    const SemanticFeature& semanticFeatureGender;
    const SemanticFeature& semanticFeatureAnimacy;

public:
    ::inflection::dialog::SpeakableString quote(const ::inflection::dialog::SpeakableString& str) const override;

public: /* package */
    ::inflection::dialog::SpeakableString quantifyType(const ::inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const override;

public:
    ::inflection::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit RuCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~RuCommonConceptFactory() override;
};
