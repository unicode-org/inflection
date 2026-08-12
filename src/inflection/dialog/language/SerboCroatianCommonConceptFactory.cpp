/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/dialog/language/SerboCroatianCommonConceptFactory.hpp>

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <cmath>

namespace inflection::dialog::language {

// Croatian (hr) and Serbian (sr) share Bosnian-Croatian-Serbian numeral-noun agreement: the 2-4
// paucal takes the genitive singular and 5+ takes the genitive plural.
SerboCroatianCommonConceptFactory::SerboCroatianCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, {
        /* few */            Agreement::PAUCAL_GOVERNED,
        /* many */           Agreement::GOVERNED_PLURAL, // BCS has no CLDR "many" category
        /* other */          Agreement::GOVERNED_PLURAL, // integer 5+ -> genitive plural
        /* animateAccusativeNumeral */ true,
    })
{
}

::inflection::dialog::SpeakableString* SerboCroatianCommonConceptFactory::quantifyFormatted(
    const NumberConcept& number, const ::inflection::dialog::SpeakableString& formattedNumber,
    const SemanticFeatureConceptBase* semanticConcept) const
{
    auto originalCountConstraint = (semanticFeatureCount == nullptr
        ? nullptr : npc(semanticConcept)->getConstraint(*npc(semanticFeatureCount)));
    const double value = number.doubleValue();
    const bool isInteger = ::std::isfinite(value) && value == ::std::floor(value);
    const Plurality::Rule countType = selectCount(value);
    // BCS spreads decimals across one/few/other, so the CLDR category cannot identify a fraction;
    // any non-integer takes the genitive singular, while integers follow the category-based agreement.
    const Agreement mode = !isInteger
        ? Agreement::FRACTION_GENITIVE_SG
        : agreementForCount(countType);
    auto* result = new SpeakableString(resolveQuantity(formattedNumber, *npc(semanticConcept), mode, countType));
    ::inflection::util::Validate::isTrue(semanticFeatureCount == nullptr
        || originalCountConstraint == npc(semanticConcept)->getConstraint(*npc(semanticFeatureCount)));
    return result;
}

void SerboCroatianCommonConceptFactory::applyGatedGenitive(
    SemanticFeatureConceptBase& clone, std::u16string_view baseCase, Agreement mode) const
{
    if (mode == Agreement::GOVERNED_PLURAL) {
        // In Serbo-Croatian, integers 5+ (and indeclinable numerals like "пет") always govern the Genitive Plural ("бродова"),
        // even inside an oblique case context (e.g., Locative, Instrumental, Dative).
        clone.putConstraint(semanticFeatureCase, ::inflection::grammar::synthesis::GrammemeConstants::CASE_GENITIVE);
    } else {
        super::applyGatedGenitive(clone, baseCase, mode);
    }
}

} // namespace inflection::dialog::language
