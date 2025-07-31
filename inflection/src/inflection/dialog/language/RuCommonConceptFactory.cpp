/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/RuCommonConceptFactory.hpp>

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/lang/StringFilterUtil.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::dialog::language {

RuCommonConceptFactory::RuCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
    , semanticFeatureCase(*npc(semanticFeatureModel.getFeature(::inflection::grammar::synthesis::GrammemeConstants::CASE)))
    , semanticFeatureGender(*npc(semanticFeatureModel.getFeature(::inflection::grammar::synthesis::GrammemeConstants::GENDER)))
    , semanticFeatureAnimacy(*npc(semanticFeatureModel.getFeature(::inflection::grammar::synthesis::GrammemeConstants::ANIMACY)))
{
}

RuCommonConceptFactory::~RuCommonConceptFactory()
{
}

::inflection::dialog::SpeakableString RuCommonConceptFactory::quote(const ::inflection::dialog::SpeakableString& str) const
{
    const auto& printStr = str.getPrint();
    if (inflection::util::UnicodeSetUtils::containsSome(::inflection::lang::StringFilterUtil::CYRILLIC_SCRIPT(), printStr)) {
        return super::quote(str);
    }
    return str;
}

::inflection::dialog::SpeakableString RuCommonConceptFactory::quantifyType(const ::inflection::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const
{
    ::std::unique_ptr<::inflection::dialog::SpeakableString> speakableResult;
    if (!useDefault) {
        ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptClone(npc(semanticConcept.clone()));
        ::std::u16string type;
        if (Plurality::Rule::ONE == countType) {
            type = ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
        } else if (Plurality::Rule::OTHER == countType) {
            type = ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
            semanticConceptClone->putConstraint(semanticFeatureCase, ::inflection::grammar::synthesis::GrammemeConstants::CASE_GENITIVE());
        } else {
            std::unique_ptr<SpeakableString> caseSpeakableString(
                semanticConceptClone->getFeatureValue(semanticFeatureCase));
            ::std::u16string caseString;
            if (caseSpeakableString != nullptr && ::inflection::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() != caseSpeakableString->getPrint()) {
                caseString = caseSpeakableString->getPrint();
            }
            if (caseString.empty() || ::inflection::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() == caseString || ::inflection::grammar::synthesis::GrammemeConstants::CASE_ACCUSATIVE() == caseString) {
                semanticConceptClone->putConstraint(semanticFeatureCase, ::inflection::grammar::synthesis::GrammemeConstants::CASE_GENITIVE());
            }
            if ((caseString.empty() || ::inflection::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() == caseString) && Plurality::Rule::FEW == countType) {
                type = ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
            } else {
                if (::inflection::grammar::synthesis::GrammemeConstants::CASE_ACCUSATIVE() == caseString && Plurality::Rule::FEW == countType) {
                    ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptCloneForAnimacy(npc(semanticConcept.clone()));
                    semanticConceptCloneForAnimacy->clearConstraint(semanticFeatureCase);
                    std::unique_ptr<SpeakableString> animacySpeakableString(
                        semanticConceptCloneForAnimacy->getFeatureValue(semanticFeatureAnimacy));
                    if (animacySpeakableString != nullptr && ::inflection::grammar::synthesis::GrammemeConstants::ANIMACY_INANIMATE() == animacySpeakableString->getPrint()) {
                        type = ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
                    } else {
                        type = ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL();
                    }
                } else {
                    type = ::inflection::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL();
                }
            }
        }
        semanticConceptClone->putConstraint(*npc(semanticFeatureCount), type);
        speakableResult.reset(semanticConceptClone->toSpeakableString());
    }
    if (speakableResult == nullptr) {
        speakableResult.reset(semanticConcept.toSpeakableString());
    }
    return quantifiedJoin(formattedNumber, *npc(speakableResult.get()), {}, countType);
}

inflection::dialog::SpeakableString* RuCommonConceptFactory::quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    std::unique_ptr<SpeakableString> genderSpeakableString(
        npc(semanticConcept)->getFeatureValue(semanticFeatureGender));
    ::std::u16string gender;
    if (genderSpeakableString != nullptr) {
        gender = genderSpeakableString->getPrint();
    }
    inflection::dialog::SpeakableString formattedNumber({});
    if (!gender.empty()) {
        std::unique_ptr<SpeakableString> caseSpeakableString(
            npc(semanticConcept)->getFeatureValue(semanticFeatureCase));
        ::std::u16string caseString;
        if (caseSpeakableString != nullptr && ::inflection::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() != caseSpeakableString->getPrint()) {
            caseString = caseSpeakableString->getPrint();
            // TODO Deprecated usage based on bad advice when Russian was first started. We can change this once RBNF is updated.
            if (caseString == ::inflection::grammar::synthesis::GrammemeConstants::CASE_INSTRUMENTAL()) {
                caseString = ::inflection::grammar::synthesis::GrammemeConstants::CASE_ABLATIVE();
            }
            else if (caseString == ::inflection::grammar::synthesis::GrammemeConstants::CASE_PREPOSITIONAL()) {
                caseString = ::inflection::grammar::synthesis::GrammemeConstants::CASE_LOCATIVE();
            }
            caseString = ::std::u16string(u"-") + caseString;
        }
        formattedNumber = number.asSpokenWords(::std::u16string(u"cardinal-") + gender + caseString);
    } else {
        // We don't know what this word is. Default to digits.
        formattedNumber = number.getAsDigits();
    }
    return super::quantifyFormatted(number, formattedNumber, semanticConcept);
}

} // namespace inflection::dialog::language
