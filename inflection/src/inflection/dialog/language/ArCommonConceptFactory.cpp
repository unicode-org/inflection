/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/ArCommonConceptFactory.hpp>

#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/npc.hpp>

using inflection::grammar::synthesis::GrammemeConstants;

namespace inflection::dialog::language {

ArCommonConceptFactory::ArCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language, ::inflection::grammar::synthesis::GrammemeConstants::GENDER, {
            {::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
            {::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"}})
    , semanticFeatureCase(*npc(semanticFeatureModel.getFeature(::inflection::grammar::synthesis::GrammemeConstants::CASE)))
{
}

inflection::dialog::SemanticConceptList* ArCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(super::createOrList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u"، أو ")))->setItemDelimiter(inflection::dialog::SpeakableString(u"، أو "));
}

inflection::dialog::SemanticConceptList* ArCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(super::createAndList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u"، و")))->setItemDelimiter(inflection::dialog::SpeakableString(u"، و"));
}

inflection::dialog::SpeakableString ArCommonConceptFactory::quantifiedJoin(const ::inflection::dialog::SpeakableString& formattedNumber, const ::inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& /*measureWord*/, Plurality::Rule countType) const
{
    if (countType == Plurality::Rule::ONE || countType == Plurality::Rule::TWO) {
        return nounPhrase;
    }
    return formattedNumber + ::inflection::dialog::SpeakableString(u" ") + nounPhrase;
}

enum class Case {
    nominative,
    genitive,
    accusative
};

static const char16_t NOMINATIVE[] = u"nominative";
static const char16_t GENITIVE[] = u"genitive";
static const char16_t ACCUSATIVE[] = u"accusative";

/*
 * The case of the quantity specified by the noun changes depending on the value being quantified.
 * Notice that ZERO, FEW and OTHER do not vary. The ONE and TWO do vary.
 */
static const struct {
    const char16_t* caseStr;
} CASE_MAP[3][6] = {
    {   // nominative
        {GENITIVE}, // ZERO
        {NOMINATIVE}, // ONE
        {NOMINATIVE}, // TWO
        {GENITIVE}, // FEW
        {ACCUSATIVE}, // MANY
        {GENITIVE}, // OTHER
    },
    {   // genitive
        {GENITIVE}, // ZERO
        {GENITIVE}, // ONE
        {GENITIVE}, // TWO
        {GENITIVE}, // FEW
        {ACCUSATIVE}, // MANY
        {GENITIVE}, // OTHER
    },
    {   // accusative
        {GENITIVE}, // ZERO
        {ACCUSATIVE}, // ONE
        {GENITIVE}, // TWO
        {GENITIVE}, // FEW
        {ACCUSATIVE}, // MANY
        {GENITIVE}, // OTHER
    }
};

inflection::dialog::SpeakableString ArCommonConceptFactory::quantifyType(const ::inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const
{
    ::std::unique_ptr<::inflection::dialog::SpeakableString> speakableResult;
    if (!useDefault) {
        ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptClone(npc(semanticConcept.clone()));
        if (Plurality::Rule::TWO == countType) {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_DUAL());
        }
        else if (Plurality::Rule::FEW == countType) {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL());
        }
        else {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR());
        }
        auto caseStr = semanticConcept.getConstraint(semanticFeatureCase);
        auto caseEnum = Case::nominative; // This is the assumed default case.
        if (caseStr != nullptr) {
            if (*npc(caseStr) == GrammemeConstants::CASE_GENITIVE()) {
                caseEnum = Case::genitive;
            }
            else if (*npc(caseStr) == GrammemeConstants::CASE_ACCUSATIVE()) {
                caseEnum = Case::accusative;
            }
        }
        semanticConceptClone->putConstraint(semanticFeatureCase, CASE_MAP[int8_t(caseEnum)][(int8_t)countType].caseStr);
        speakableResult.reset(semanticConceptClone->toSpeakableString());
    }
    if (speakableResult == nullptr) {
        speakableResult.reset(semanticConcept.toSpeakableString());
    }
    return quantifiedJoin(formattedNumber, *npc(speakableResult.get()), {}, countType);
}

} // namespace inflection::dialog::language
