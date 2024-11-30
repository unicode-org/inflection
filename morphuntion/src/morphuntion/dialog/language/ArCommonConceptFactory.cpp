/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/ArCommonConceptFactory.hpp>

#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/npc.hpp>

using morphuntion::grammar::synthesis::GrammemeConstants;

namespace morphuntion::dialog::language {

ArCommonConceptFactory::ArCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language, ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER, {
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE(), u"cardinal-masculine"},
            {::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE(), u"cardinal-feminine"}})
    , semanticFeatureCase(*npc(semanticFeatureModel.getFeature(::morphuntion::grammar::synthesis::GrammemeConstants::CASE)))
{
}

morphuntion::dialog::SemanticConceptList* ArCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(super::createOrList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u"، أو ")))->setItemDelimiter(morphuntion::dialog::SpeakableString(u"، أو "));
}

morphuntion::dialog::SemanticConceptList* ArCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(npc(super::createAndList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u"، و")))->setItemDelimiter(morphuntion::dialog::SpeakableString(u"، و"));
}

morphuntion::dialog::SpeakableString ArCommonConceptFactory::quantifiedJoin(const ::morphuntion::dialog::SpeakableString& formattedNumber, const ::morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& /*measureWord*/, Plurality::Rule countType) const
{
    if (countType == Plurality::Rule::ONE || countType == Plurality::Rule::TWO) {
        return nounPhrase;
    }
    return formattedNumber + ::morphuntion::dialog::SpeakableString(u" ") + nounPhrase;
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

morphuntion::dialog::SpeakableString ArCommonConceptFactory::quantifyType(const ::morphuntion::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const
{
    ::std::unique_ptr<::morphuntion::dialog::SpeakableString> speakableResult;
    if (!useDefault) {
        ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptClone(npc(semanticConcept.clone()));
        if (Plurality::Rule::TWO == countType) {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_DUAL());
        }
        else if (Plurality::Rule::FEW == countType) {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL());
        }
        else {
            semanticConceptClone->putConstraint(*npc(semanticFeatureCount), ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR());
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

} // namespace morphuntion::dialog::language
