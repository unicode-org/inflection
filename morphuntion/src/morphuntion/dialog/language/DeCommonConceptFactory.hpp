/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::DeCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;
private:
    const SemanticFeature* semanticFeatureGender {  };
    const SemanticFeature* semanticFeatureCase {  };
public:
    ::morphuntion::dialog::SpeakableString quote(const ::morphuntion::dialog::SpeakableString& str) const override;
    morphuntion::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit DeCommonConceptFactory(const ::morphuntion::util::ULocale& language);
};
