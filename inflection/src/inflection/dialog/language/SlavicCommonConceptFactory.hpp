/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>
#include <optional>
#include <string>
#include <string_view>

class inflection::dialog::language::SlavicCommonConceptFactory
    : public CommonConceptFactoryImpl
{
public:
    typedef CommonConceptFactoryImpl super;

    enum class Agreement {
        SINGULAR_BASE,
        PAUCAL_GOVERNED,
        PAUCAL_PLURAL,
        GOVERNED_PLURAL,
        FRACTION_GENITIVE_SG,
    };

    struct Config {
        Agreement few {};
        Agreement many {};
        Agreement other {};
        bool animateAccusativeNumeral {};
    };

protected:
    const SemanticFeature& semanticFeatureCase;
    const SemanticFeature& semanticFeatureGender;
    const SemanticFeature& semanticFeatureAnimacy;
    const Config config;
    const bool quoteCyrillic; // derived from the locale's script (Cyrillic vs Latin)

protected:
    static std::u16string getFeature(const SemanticFeatureConceptBase& semanticConcept, const SemanticFeature& semanticFeature);
    Agreement agreementForCount(Plurality::Rule countType) const;
    bool isDirectCase(std::u16string_view baseCase, std::u16string_view animacy) const;
    void applyGatedGenitive(SemanticFeatureConceptBase& clone, std::u16string_view baseCase) const;

    virtual std::optional<std::u16string> adjustCase(Plurality::Rule countType,
        const SemanticFeatureConceptBase& semanticConcept, std::u16string_view baseCase) const;
    virtual std::u16string buildNumeralRuleName(const SemanticFeatureConceptBase& semanticConcept,
        std::u16string_view gender, std::u16string_view baseCase) const;

    // Applies the chosen agreement strategy to the counted noun and joins it with the number.
    ::inflection::dialog::SpeakableString resolveQuantity(
        const ::inflection::dialog::SpeakableString& formattedNumber,
        const SemanticFeatureConceptBase& semanticConcept,
        Agreement mode, Plurality::Rule countType) const;

    // Maps the plural category to an agreement strategy via the config. The number-only seam
    // is sufficient for languages whose CLDR rules separate integers from fractions; Serbo-Croatian,
    // which lacks a "many" category, overrides quantifyFormatted to split "other" by integrality.
    ::inflection::dialog::SpeakableString quantifyType(const ::inflection::dialog::SpeakableString& formattedNumber,
        const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const override;

public:
    ::inflection::dialog::SpeakableString quote(const ::inflection::dialog::SpeakableString& str) const override;
    ::inflection::dialog::SpeakableString* quantify(const NumberConcept& number,
        const SemanticFeatureConceptBase* semanticConcept) const override;

    SlavicCommonConceptFactory(const ::inflection::util::ULocale& language, const Config& config);
    SlavicCommonConceptFactory(const SlavicCommonConceptFactory&) = delete;
    SlavicCommonConceptFactory& operator=(const SlavicCommonConceptFactory&) = delete;
};
