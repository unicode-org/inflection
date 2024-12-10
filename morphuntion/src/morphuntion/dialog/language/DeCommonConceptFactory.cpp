/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/DeCommonConceptFactory.hpp>

#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog::language {

DeCommonConceptFactory::DeCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
{
    this->semanticFeatureGender = semanticFeatureModel.getFeature(::morphuntion::grammar::synthesis::GrammemeConstants::GENDER);
    this->semanticFeatureCase = semanticFeatureModel.getFeature(::morphuntion::grammar::synthesis::GrammemeConstants::CASE);
}

::morphuntion::dialog::SpeakableString DeCommonConceptFactory::quote(const ::morphuntion::dialog::SpeakableString& str) const
{
    if (::morphuntion::util::LocaleUtils::SWITZERLAND_GERMAN() == getLanguage()) {
        if (str.speakEqualsPrint()) {
            return ::morphuntion::dialog::SpeakableString(u"«" + str.getPrint() + u"»");
        }
        return ::morphuntion::dialog::SpeakableString(u"«" + str.getPrint() + u"»", str.getSpeak());
    }
    return super::quote(str);
}

morphuntion::dialog::SpeakableString* DeCommonConceptFactory::quantify(const NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    auto genderSpeakableString = npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureGender));
    ::std::u16string gender;
    if (genderSpeakableString != nullptr) {
        gender = npc(genderSpeakableString)->getPrint();
        delete genderSpeakableString;
    }
    auto caseSpeakableString = npc(semanticConcept)->getFeatureValue(*npc(semanticFeatureCase));
    ::std::u16string caseString;
    if (caseSpeakableString != nullptr) {
        caseString = npc(caseSpeakableString)->getPrint();
        delete caseSpeakableString;
    }
    ::morphuntion::dialog::SpeakableString formattedNumber({});
    if (caseString.empty() || ::morphuntion::grammar::synthesis::GrammemeConstants::CASE_NOMINATIVE() == caseString) {
        if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-masculine");
        } else if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-feminine");
        } else if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-neuter");
        }
    } else if (::morphuntion::grammar::synthesis::GrammemeConstants::CASE_GENITIVE() == caseString) {
        if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender || ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-s");
        } else if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-r");
        }
    } else if (::morphuntion::grammar::synthesis::GrammemeConstants::CASE_DATIVE() == caseString) {
        if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender || ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-m");
        } else if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-r");
        }
    } else if (::morphuntion::grammar::synthesis::GrammemeConstants::CASE_ACCUSATIVE() == caseString) {
        if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-n");
        } else if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-feminine");
        } else if (::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_NEUTER() == gender) {
            formattedNumber = number.asSpokenWords(u"cardinal-neuter");
        }
    }
    if (formattedNumber.isEmpty()) {
        formattedNumber = number.getAsDigits();
    }
    return super::quantify(number, formattedNumber, semanticConcept);
}

} // namespace morphuntion::dialog::language
