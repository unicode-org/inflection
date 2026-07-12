/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/language/SlavicCommonConceptFactory.hpp>
#include <optional>
#include <string>
#include <string_view>

class inflection::dialog::language::PlCommonConceptFactory
    : public SlavicCommonConceptFactory
{
public:
    typedef SlavicCommonConceptFactory super;

protected:
    std::optional<std::u16string> adjustCase(Plurality::Rule countType,
        const SemanticFeatureConceptBase& semanticConcept, std::u16string_view baseCase) const override;
    std::u16string buildNumeralRuleName(const SemanticFeatureConceptBase& semanticConcept,
        std::u16string_view gender, std::u16string_view baseCase) const override;

public:
    explicit PlCommonConceptFactory(const ::inflection::util::ULocale& language);
};
