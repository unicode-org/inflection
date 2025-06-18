/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/DeCommonConceptFactory.hpp>

#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog::language {

DeCommonConceptFactory::DeCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
{
    this->semanticFeatureGender = semanticFeatureModel.getFeature(::inflection::grammar::synthesis::GrammemeConstants::GENDER);
    this->semanticFeatureCase = semanticFeatureModel.getFeature(::inflection::grammar::synthesis::GrammemeConstants::CASE);
}

::inflection::dialog::SpeakableString DeCommonConceptFactory::quote(const ::inflection::dialog::SpeakableString& str) const
{
    if (::inflection::util::LocaleUtils::SWITZERLAND_GERMAN() == getLanguage()) {
        if (str.speakEqualsPrint()) {
            return ::inflection::dialog::SpeakableString(u"«" + str.getPrint() + u"»");
        }
        return ::inflection::dialog::SpeakableString(u"«" + str.getPrint() + u"»", str.getSpeak());
    }
    return super::quote(str);
}

inflection::dialog::SpeakableString* DeCommonConceptFactory::quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    std::unique_ptr<SpeakableString> genderSpeakableString(
        npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureGender)));
    ::std::u16string gender;
    if (genderSpeakableString != nullptr) {
        gender = genderSpeakableString->getPrint();
    }
    std::unique_ptr<SpeakableString> caseSpeakableString(
        npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureCase)));
    ::std::u16string caseString;
    if (caseSpeakableString != nullptr) {
        caseString = caseSpeakableString->getPrint();
    }
    ::inflection::dialog::SpeakableString formattedNumber({});
    if (caseString.empty() || ::inflection::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() == caseString) {
        if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-masculine");
        } else if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-feminine");
        } else if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-neuter");
        }
    } else if (::inflection::grammar::synthesis::GrammemeConstants::CASE_GENITIVE() == caseString) {
        if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender || ::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-s");
        } else if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-r");
        }
    } else if (::inflection::grammar::synthesis::GrammemeConstants::CASE_DATIVE() == caseString) {
        if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender || ::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-m");
        } else if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-r");
        }
    } else if (::inflection::grammar::synthesis::GrammemeConstants::CASE_ACCUSATIVE() == caseString) {
        if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-n");
        } else if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-feminine");
        } else if (::inflection::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-neuter");
        }
    }
    if (formattedNumber.isEmpty()) {
        formattedNumber = number.getAsDigits();
    }
    return super::quantify(number, formattedNumber, semanticConcept);
}

} // namespace inflection::dialog::language
