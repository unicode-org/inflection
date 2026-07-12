/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/dialog/language/SlavicCommonConceptFactory.hpp>

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <unicode/uscript.h>
#include <memory>

namespace inflection::dialog::language {

using ::inflection::grammar::synthesis::GrammemeConstants;

static bool isCyrillicLocale(const ::inflection::util::ULocale& language) {
    UErrorCode status = U_ZERO_ERROR;
    UScriptCode scripts[4] = { USCRIPT_INVALID_CODE };
    // uscript_getCode resolves the locale's script via likely subtags (e.g. ru/uk/sr -> Cyrillic, pl/cs/sk/hr -> Latin).
    const int32_t count = uscript_getCode(language.getName().c_str(), scripts, 4, &status);
    return U_SUCCESS(status) && count > 0 && scripts[0] == USCRIPT_CYRILLIC;
}

SlavicCommonConceptFactory::SlavicCommonConceptFactory(const ::inflection::util::ULocale& language, const Config& config)
    : super(language)
    , semanticFeatureCase(*npc(semanticFeatureModel.getFeature(GrammemeConstants::CASE)))
    , semanticFeatureGender(*npc(semanticFeatureModel.getFeature(GrammemeConstants::GENDER)))
    , semanticFeatureAnimacy(*npc(semanticFeatureModel.getFeature(GrammemeConstants::ANIMACY)))
    , config(config)
    , quoteCyrillic(isCyrillicLocale(language))
{
}

std::u16string SlavicCommonConceptFactory::getFeature(const SemanticFeatureConceptBase& semanticConcept, const SemanticFeature& semanticFeature)
{
    ::std::unique_ptr<SpeakableString> v(semanticConcept.getFeatureValue(semanticFeature));
    return v != nullptr ? v->getPrint() : std::u16string();
}

bool SlavicCommonConceptFactory::isDirectCase(std::u16string_view baseCase, std::u16string_view animacy) const
{
    if (baseCase.empty() || baseCase == GrammemeConstants::CASE_NOMINATIVE) {
        return true;
    }
    return baseCase == GrammemeConstants::CASE_ACCUSATIVE && animacy == GrammemeConstants::ANIMACY_INANIMATE;
}

void SlavicCommonConceptFactory::applyGatedGenitive(SemanticFeatureConceptBase& clone, std::u16string_view baseCase) const
{
    if (baseCase.empty() || baseCase == GrammemeConstants::CASE_NOMINATIVE || baseCase == GrammemeConstants::CASE_ACCUSATIVE) {
        clone.putConstraint(semanticFeatureCase, GrammemeConstants::CASE_GENITIVE);
    }
}

SlavicCommonConceptFactory::Agreement
SlavicCommonConceptFactory::agreementForCount(Plurality::Rule countType) const
{
    switch (countType) {
        case Plurality::Rule::ONE:
            return Agreement::SINGULAR_BASE;
        case Plurality::Rule::FEW:
            return config.few;
        case Plurality::Rule::MANY:
            return config.many;
        case Plurality::Rule::OTHER:
        default: // ZERO / TWO never occur for these locales; treat as `other`.
            return config.other;
    }
}

std::optional<std::u16string> SlavicCommonConceptFactory::adjustCase(
    Plurality::Rule, const SemanticFeatureConceptBase&, std::u16string_view) const
{
    return std::nullopt;
}

std::u16string SlavicCommonConceptFactory::buildNumeralRuleName(
    const SemanticFeatureConceptBase& semanticConcept, std::u16string_view gender, std::u16string_view baseCase) const
{
    std::u16string ruleName(u"cardinal-");
    ruleName += gender;
    if (config.animateAccusativeNumeral
        && gender == GrammemeConstants::GENDER_MASCULINE && baseCase == GrammemeConstants::CASE_ACCUSATIVE)
    {
        // Animacy is only consulted for the masculine-accusative numeral form.
        const std::u16string animacy(getFeature(semanticConcept, semanticFeatureAnimacy));
        if (animacy == GrammemeConstants::ANIMACY_ANIMATE || animacy == GrammemeConstants::ANIMACY_HUMAN) {
            return ruleName + std::u16string(u"-animate-accusative");
        }
    }
    if (!baseCase.empty() && baseCase != GrammemeConstants::CASE_NOMINATIVE) {
        ruleName += std::u16string(u"-") + std::u16string(baseCase);
    }
    return ruleName;
}

::inflection::dialog::SpeakableString SlavicCommonConceptFactory::resolveQuantity(
    const ::inflection::dialog::SpeakableString& formattedNumber,
    const SemanticFeatureConceptBase& semanticConcept, Agreement mode, Plurality::Rule countType) const
{
    ::std::unique_ptr<SemanticFeatureConceptBase> clone(npc(semanticConcept.clone()));
    const std::u16string baseCase(getFeature(semanticConcept, semanticFeatureCase));

    std::u16string numberConstraint(GrammemeConstants::NUMBER_SINGULAR);
    switch (mode) {
        case Agreement::SINGULAR_BASE: {
            numberConstraint = GrammemeConstants::NUMBER_SINGULAR;
            // adjustCase queries gender/animacy itself, only when it actually needs them.
            if (auto adj = adjustCase(countType, semanticConcept, baseCase)) {
                clone->putConstraint(semanticFeatureCase, *adj);
            }
            break;
        }
        case Agreement::PAUCAL_GOVERNED: {
            // Animacy is needed here to tell the direct (singular paucal) case from the oblique plural.
            const std::u16string animacy(getFeature(semanticConcept, semanticFeatureAnimacy));
            numberConstraint = isDirectCase(baseCase, animacy) ? GrammemeConstants::NUMBER_SINGULAR : GrammemeConstants::NUMBER_PLURAL;
            applyGatedGenitive(*clone, baseCase);
            break;
        }
        case Agreement::PAUCAL_PLURAL: {
            numberConstraint = GrammemeConstants::NUMBER_PLURAL;
            if (auto adj = adjustCase(countType, semanticConcept, baseCase)) {
                clone->putConstraint(semanticFeatureCase, *adj);
            }
            break;
        }
        case Agreement::GOVERNED_PLURAL: {
            numberConstraint = GrammemeConstants::NUMBER_PLURAL;
            applyGatedGenitive(*clone, baseCase);
            break;
        }
        case Agreement::FRACTION_GENITIVE_SG: {
            numberConstraint = GrammemeConstants::NUMBER_SINGULAR;
            clone->putConstraint(semanticFeatureCase, GrammemeConstants::CASE_GENITIVE);
            break;
        }
    }
    clone->putConstraint(*npc(semanticFeatureCount), numberConstraint);
    ::std::unique_ptr<SpeakableString> result(clone->toSpeakableString());
    if (result == nullptr) {
        // The constrained form could not be rendered (e.g. missing dictionary form);
        // fall back to the unconstrained concept, mirroring the original Ru/Pl behavior.
        result.reset(semanticConcept.toSpeakableString());
    }
    return quantifiedJoin(formattedNumber, *npc(result.get()), {}, countType);
}

::inflection::dialog::SpeakableString SlavicCommonConceptFactory::quote(const ::inflection::dialog::SpeakableString& str) const
{
    if (quoteCyrillic && !::inflection::util::UnicodeSetUtils::containsSome(USCRIPT_CYRILLIC, str.getPrint())) {
        return str;
    }
    return super::quote(str);
}

::inflection::dialog::SpeakableString* SlavicCommonConceptFactory::quantify(
    const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    const std::u16string gender(getFeature(*npc(semanticConcept), semanticFeatureGender));
    ::inflection::dialog::SpeakableString formattedNumber({});
    if (!gender.empty()) {
        const std::u16string baseCase(getFeature(*npc(semanticConcept), semanticFeatureCase));
        formattedNumber = number.asSpokenWords(buildNumeralRuleName(*npc(semanticConcept), gender, baseCase));
    } else {
        formattedNumber = number.getAsDigits();
    }
    return quantifyFormatted(number, formattedNumber, semanticConcept);
}

::inflection::dialog::SpeakableString SlavicCommonConceptFactory::quantifyType(
    const ::inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept,
    bool useDefault, Plurality::Rule countType) const
{
    if (useDefault) {
        return super::quantifyType(formattedNumber, semanticConcept, useDefault, countType);
    }
    return resolveQuantity(formattedNumber, semanticConcept, agreementForCount(countType), countType);
}

} // namespace inflection::dialog::language
