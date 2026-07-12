/*
 * Copyright 2025-2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/dialog/language/PlCommonConceptFactory.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>

namespace inflection::dialog::language {

using ::inflection::grammar::synthesis::GrammemeConstants;

PlCommonConceptFactory::PlCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, {
        /* few */            Agreement::PAUCAL_PLURAL,
        /* many */           Agreement::GOVERNED_PLURAL,
        /* other */          Agreement::FRACTION_GENITIVE_SG,
        /* animateAccusativeNumeral */ false, // Polish overrides buildNumeralRuleName; flag unused
    })
{
}

// Reproduces the accusative case adjustment of the original PlCommonConceptFactory::quantifyType.
std::optional<std::u16string> PlCommonConceptFactory::adjustCase(
    Plurality::Rule countType, const SemanticFeatureConceptBase& semanticConcept, std::u16string_view baseCase) const
{
    if (baseCase != GrammemeConstants::CASE_ACCUSATIVE) {
        return std::nullopt; // gender/animacy are only relevant in the accusative
    }
    const std::u16string gender(getFeature(semanticConcept, semanticFeatureGender));
    if (countType == Plurality::Rule::ONE) {
        if (gender == GrammemeConstants::GENDER_NEUTER) {
            return std::u16string(GrammemeConstants::CASE_NOMINATIVE);
        }
        if (gender == GrammemeConstants::GENDER_MASCULINE) {
            const std::u16string animacy(getFeature(semanticConcept, semanticFeatureAnimacy));
            if (animacy == GrammemeConstants::ANIMACY_HUMAN || animacy == GrammemeConstants::ANIMACY_ANIMATE) {
                return std::u16string(GrammemeConstants::CASE_GENITIVE);
            }
            return std::u16string(GrammemeConstants::CASE_NOMINATIVE);
        }
        return std::nullopt;
    }
    if (countType == Plurality::Rule::FEW) {
        if (gender == GrammemeConstants::GENDER_MASCULINE
            && getFeature(semanticConcept, semanticFeatureAnimacy) == GrammemeConstants::ANIMACY_HUMAN) {
            return std::u16string(GrammemeConstants::CASE_GENITIVE);
        }
        return std::u16string(GrammemeConstants::CASE_NOMINATIVE);
    }
    return std::nullopt;
}

// Reproduces the numeral rule-name scheme of the original PlCommonConceptFactory::quantify.
std::u16string PlCommonConceptFactory::buildNumeralRuleName(
    const SemanticFeatureConceptBase& semanticConcept, std::u16string_view gender, std::u16string_view baseCase) const
{
    std::u16string result(u"cardinal-");
    result += gender;
    std::u16string caseStr;
    if (baseCase != GrammemeConstants::CASE_NOMINATIVE) {
        caseStr = baseCase;
    }
    if (gender == GrammemeConstants::GENDER_MASCULINE) {
        // The personal/animate numeral variants exist only for the masculine gender.
        const std::u16string animacy(getFeature(semanticConcept, semanticFeatureAnimacy));
        if ((caseStr.empty() || caseStr == GrammemeConstants::CASE_ACCUSATIVE)
            && animacy == GrammemeConstants::ANIMACY_HUMAN)
        {
            if (!caseStr.empty()) {
                result += u"-" + caseStr;
            }
            result += u"-personal";
            return result;
        }
        if (caseStr == GrammemeConstants::CASE_ACCUSATIVE && animacy == GrammemeConstants::ANIMACY_ANIMATE) {
            return result + u"-accusative-animate";
        }
    }
    if (!caseStr.empty()) {
        result += u"-" + caseStr;
    }
    return result;
}

} // namespace inflection::dialog::language
