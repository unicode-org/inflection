/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/language/fwd.hpp>
#include <morphuntion/dialog/CommonConceptFactoryImpl.hpp>

class morphuntion::dialog::language::RuCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

private:
    const SemanticFeature& semanticFeatureCase;
    const SemanticFeature& semanticFeatureGender;
    const SemanticFeature& semanticFeatureAnimacy;

public:
    ::morphuntion::dialog::SpeakableString quote(const ::morphuntion::dialog::SpeakableString& str) const override;

public: /* package */
    ::morphuntion::dialog::SpeakableString quantifyType(const ::morphuntion::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const override;

public:
    ::morphuntion::dialog::SpeakableString* quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit RuCommonConceptFactory(const ::morphuntion::util::ULocale& language);
    ~RuCommonConceptFactory() override;
};
