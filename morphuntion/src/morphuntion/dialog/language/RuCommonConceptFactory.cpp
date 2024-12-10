/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/RuCommonConceptFactory.hpp>

#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/lang/StringFilterUtil.hpp>
#include <morphuntion/util/UnicodeSetUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::dialog::language {

RuCommonConceptFactory::RuCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
    , semanticFeatureCase(*npc(semanticFeatureModel.getFeature(::morphuntion::grammar::synthesis::GrammemeConstants::CASE)))
    , semanticFeatureGender(*npc(semanticFeatureModel.getFeature(::morphuntion::grammar::synthesis::GrammemeConstants::GENDER)))
    , semanticFeatureAnimacy(*npc(semanticFeatureModel.getFeature(::morphuntion::grammar::synthesis::GrammemeConstants::ANIMACY)))
{
}

RuCommonConceptFactory::~RuCommonConceptFactory()
{
}

::morphuntion::dialog::SpeakableString RuCommonConceptFactory::quote(const ::morphuntion::dialog::SpeakableString& str) const
{
    const auto& printStr = str.getPrint();
    if (morphuntion::util::UnicodeSetUtils::containsSome(::morphuntion::lang::StringFilterUtil::CYRILLIC_SCRIPT(), printStr)) {
        return super::quote(str);
    }
    return str;
}

::morphuntion::dialog::SpeakableString RuCommonConceptFactory::quantifyType(const ::morphuntion::dialog::SpeakableString& formattedNumber, const SemanticFeatureConceptBase& semanticConcept, bool useDefault, Plurality::Rule countType) const
{
    ::std::unique_ptr<::morphuntion::dialog::SpeakableString> speakableResult;
    if (!useDefault) {
        ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptClone(npc(semanticConcept.clone()));
        ::std::u16string type;
        if (Plurality::Rule::ONE == countType) {
            type = ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
        } else if (Plurality::Rule::OTHER == countType) {
            type = ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
            semanticConceptClone->putConstraint(semanticFeatureCase, ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_GENITIVE());
        } else {
            auto caseSpeakableString = semanticConceptClone->getFeatureValue(semanticFeatureCase);
            ::std::u16string caseString;
            if (caseSpeakableString != nullptr && ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() != npc(caseSpeakableString)->getPrint()) {
                caseString = npc(caseSpeakableString)->getPrint();
            }
            delete caseSpeakableString;
            if (caseString.empty() || ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() == caseString || ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_ACCUSATIVE() == caseString) {
                semanticConceptClone->putConstraint(semanticFeatureCase, ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_GENITIVE());
            }
            if ((caseString.empty() || ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() == caseString) && Plurality::Rule::FEW == countType) {
                type = ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
            } else {
                if (::morphuntion::grammar::synthesis::GrammemeConstants::CASE_ACCUSATIVE() == caseString && Plurality::Rule::FEW == countType) {
                    ::std::unique_ptr<SemanticFeatureConceptBase> semanticConceptCloneForAnimacy(npc(semanticConcept.clone()));
                    semanticConceptCloneForAnimacy->clearConstraint(semanticFeatureCase);
                    auto animacySpeakableString = semanticConceptCloneForAnimacy->getFeatureValue(semanticFeatureAnimacy);
                    if (animacySpeakableString != nullptr && ::morphuntion::grammar::synthesis::GrammemeConstants::ANIMACY_INANIMATE() == npc(animacySpeakableString)->getPrint()) {
                        type = ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_SINGULAR();
                    } else {
                        type = ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL();
                    }
                    delete animacySpeakableString;
                } else {
                    type = ::morphuntion::grammar::synthesis::GrammemeConstants::NUMBER_PLURAL();
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

morphuntion::dialog::SpeakableString* RuCommonConceptFactory::quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    auto genderSpeakableString = npc(semanticConcept)->getFeatureValue(semanticFeatureGender);
    ::std::u16string gender;
    if (genderSpeakableString != nullptr) {
        gender = npc(genderSpeakableString)->getPrint();
        delete genderSpeakableString;
    }
    morphuntion::dialog::SpeakableString formattedNumber({});
    if (!gender.empty()) {
        auto caseSpeakableString = npc(semanticConcept)->getFeatureValue(semanticFeatureCase);
        ::std::u16string caseString;
        if (caseSpeakableString != nullptr && ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() != npc(caseSpeakableString)->getPrint()) {
            caseString = npc(caseSpeakableString)->getPrint();
            // TODO Deprecated usage based on bad advice when Russian was first started. We can change this once RBNF is updated.
            if (caseString == ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_INSTRUMENTAL()) {
                caseString = ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_ABLATIVE();
            }
            else if (caseString == ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_PREPOSITIONAL()) {
                caseString = ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_LOCATIVE();
            }
            caseString = ::std::u16string(u"-") + caseString;
        }
        delete caseSpeakableString;
        formattedNumber = number.asSpokenWords(::std::u16string(u"cardinal-") + gender + caseString);
    } else {
        // We don't know what this word is. Default to digits.
        formattedNumber = number.getAsDigits();
    }
    return super::quantify(number, formattedNumber, semanticConcept);
}

} // namespace morphuntion::dialog::language
