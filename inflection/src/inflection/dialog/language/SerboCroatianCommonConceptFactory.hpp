/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/language/SlavicCommonConceptFactory.hpp>

class inflection::dialog::language::SerboCroatianCommonConceptFactory
    : public SlavicCommonConceptFactory
{
public:
    typedef SlavicCommonConceptFactory super;

    // Bosnian-Croatian-Serbian has no CLDR "many" category, so the "other" category conflates
    // large integers (genitive plural) and fractions (genitive singular). This override inspects
    // the actual number to disambiguate them before delegating to the shared resolution.
    ::inflection::dialog::SpeakableString* quantifyFormatted(const NumberConcept& number,
        const ::inflection::dialog::SpeakableString& formattedNumber,
        const SemanticFeatureConceptBase* semanticConcept) const override;

    explicit SerboCroatianCommonConceptFactory(const ::inflection::util::ULocale& language);

protected:
    void applyGatedGenitive(SemanticFeatureConceptBase& clone, std::u16string_view baseCase, Agreement mode) const override;
};
