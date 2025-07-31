/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/language/HeCommonConceptFactory.hpp>

#include <inflection/dialog/language/HeCommonConceptFactory_HeAndList.hpp>
#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SemanticConceptList.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/grammar/synthesis/GrammemeConstants.hpp>
#include <inflection/resources/DataResource.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/npc.hpp>
#include <memory>

namespace inflection::dialog::language {

HeCommonConceptFactory::HeCommonConceptFactory(const ::inflection::util::ULocale& language)
    : super(language)
    , dontQuantify({
        u"חודש",
        u"חודשיים",
        u"יומיים"
    })
    , semanticFeatureGender(*npc(semanticFeatureModel.getFeature(inflection::grammar::synthesis::GrammemeConstants::GENDER)))
    , semanticFeatureDefiniteness(*npc(semanticFeatureModel.getFeature(inflection::grammar::synthesis::GrammemeConstants::DEFINITENESS)))
{
}

HeCommonConceptFactory::~HeCommonConceptFactory()
{
}

inflection::dialog::SemanticConceptList* HeCommonConceptFactory::createOrList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(inflection::dialog::SpeakableString(u" או "));
}

inflection::dialog::SemanticConceptList* HeCommonConceptFactory::createAndList(const ::std::vector<const ::inflection::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new HeCommonConceptFactory_HeAndList(getSemanticFeatureModel(), concepts);
}

bool HeCommonConceptFactory::isDefinite(const SemanticFeatureConceptBase* semanticConcept) const
{
    auto definiteness = npc(semanticConcept)->getConstraint(semanticFeatureDefiniteness);
    return definiteness != nullptr && ::inflection::grammar::synthesis::GrammemeConstants::DEFINITENESS_DEFINITE() == *npc(definiteness);
}

inflection::dialog::SpeakableString* HeCommonConceptFactory::quantify(const inflection::dialog::NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    ::std::unique_ptr<::inflection::dialog::SpeakableString> gender(npc(semanticConcept)->getFeatureValue(semanticFeatureGender));
    ::inflection::dialog::SpeakableString newFormattedNumber({});
    if (gender != nullptr && gender->getPrint() == ::inflection::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE()) {
        if (isDefinite(semanticConcept)) {
            if (number.longValue() == 1){
                newFormattedNumber = ::inflection::dialog::SpeakableString(u"1", u"היחיד");
            }
            else {
                newFormattedNumber = number.asSpokenWords(u"construct-masculine");
            }
        }
        else {
            newFormattedNumber = number.asSpokenWords(u"cardinal-masculine");
        }
    }
    else if (gender != nullptr && gender->getPrint() == ::inflection::grammar::synthesis::GrammemeConstants::GENDER_FEMININE()) {
        if (isDefinite(semanticConcept)) {
            if (number.longValue() == 1){
                newFormattedNumber = ::inflection::dialog::SpeakableString(u"1", u"היחידה");
            }
            else {
                newFormattedNumber = number.asSpokenWords(u"construct-feminine");
            }
        }
        else {
            newFormattedNumber = number.asSpokenWords(u"cardinal-feminine");
        }
    }
    else {
        throw ::inflection::exception::IllegalStateException(::std::u16string(u"Unexpected gender: ") + (gender == nullptr ? u"null" : gender->toString()));
    }

    if (number.longValue() == 1 || number.longValue() == 2) {
        newFormattedNumber = ::inflection::dialog::SpeakableString(newFormattedNumber.getSpeak(), newFormattedNumber.getSpeak());
    }
    return super::quantifyFormatted(number, newFormattedNumber, semanticConcept);
}

inflection::dialog::SpeakableString HeCommonConceptFactory::quantifiedJoin(const inflection::dialog::SpeakableString& formattedNumber, const inflection::dialog::SpeakableString& nounPhrase, const ::std::u16string& /*measureWord*/, Plurality::Rule countType) const
{
    if (dontQuantify.find(nounPhrase.getPrint()) != dontQuantify.end()) {
        return nounPhrase;
    }
    if (countType == Plurality::Rule::ONE) {
        return nounPhrase + ::inflection::dialog::SpeakableString(u" ") + formattedNumber;
    }
    else {
        return formattedNumber + ::inflection::dialog::SpeakableString(u" ") + nounPhrase;
    }

}

} // namespace inflection::dialog::language
