/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>

class inflection::dialog::language::DeCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
private:
    const SemanticFeature* semanticFeatureGender {  };
    const SemanticFeature* semanticFeatureCase {  };
public:
    ::inflection::dialog::SpeakableString quote(const ::inflection::dialog::SpeakableString& str) const override;
    inflection::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit DeCommonConceptFactory(const ::inflection::util::ULocale& language);
};
