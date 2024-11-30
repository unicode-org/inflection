/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/language/HeCommonConceptFactory.hpp>

#include <morphuntion/dialog/language/HeCommonConceptFactory_HeAndList.hpp>
#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SemanticConceptList.hpp>
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/exception/IllegalStateException.hpp>
#include <morphuntion/grammar/synthesis/GrammemeConstants.hpp>
#include <morphuntion/resources/DataResource.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <morphuntion/npc.hpp>
#include <memory>

namespace morphuntion::dialog::language {

HeCommonConceptFactory::HeCommonConceptFactory(const ::morphuntion::util::ULocale& language)
    : super(language)
    , dontQuantify({
        u"חודש",
        u"חודשיים",
        u"יומיים"
    })
    , semanticFeatureGender(*npc(semanticFeatureModel.getFeature(morphuntion::grammar::synthesis::GrammemeConstants::GENDER)))
    , semanticFeatureDefiniteness(*npc(semanticFeatureModel.getFeature(morphuntion::grammar::synthesis::GrammemeConstants::DEFINITENESS)))
{
}

HeCommonConceptFactory::~HeCommonConceptFactory()
{
}

morphuntion::dialog::SemanticConceptList* HeCommonConceptFactory::createOrList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return npc(super::createOrList(concepts))->setBeforeLast(morphuntion::dialog::SpeakableString(u" או "));
}

morphuntion::dialog::SemanticConceptList* HeCommonConceptFactory::createAndList(const ::std::vector<const ::morphuntion::dialog::SemanticFeatureConceptBase*>& concepts) const
{
    return new HeCommonConceptFactory_HeAndList(getSemanticFeatureModel(), concepts);
}

bool HeCommonConceptFactory::isDefinite(const SemanticFeatureConceptBase* semanticConcept) const
{
    auto definiteness = npc(semanticConcept)->getConstraint(semanticFeatureDefiniteness);
    return definiteness != nullptr && ::morphuntion::grammar::synthesis::GrammemeConstants::DEFINITENESS_DEFINITE() == *npc(definiteness);
}

morphuntion::dialog::SpeakableString* HeCommonConceptFactory::quantify(const morphuntion::dialog::NumberConcept& number, const SemanticFeatureConceptBase* semanticConcept) const
{
    ::std::unique_ptr<::morphuntion::dialog::SpeakableString> gender(npc(semanticConcept)->getFeatureValue(semanticFeatureGender));
    ::morphuntion::dialog::SpeakableString newFormattedNumber({});
    if (gender != nullptr && gender->getPrint() == ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_MASCULINE()) {
        if (isDefinite(semanticConcept)) {
            if (number.longValue() == 1){
                newFormattedNumber = ::morphuntion::dialog::SpeakableString(u"1", u"היחיד");
            }
            else {
                newFormattedNumber = number.asSpokenWords(u"construct-masculine");
            }
        }
        else {
            newFormattedNumber = number.asSpokenWords(u"cardinal-masculine");
        }
    }
    else if (gender != nullptr && gender->getPrint() == ::morphuntion::grammar::synthesis::GrammemeConstants::GENDER_FEMININE()) {
        if (isDefinite(semanticConcept)) {
            if (number.longValue() == 1){
                newFormattedNumber = ::morphuntion::dialog::SpeakableString(u"1", u"היחידה");
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
        throw ::morphuntion::exception::IllegalStateException(::std::u16string(u"Unexpected gender: ") + (gender == nullptr ? u"null" : gender->toString()));
    }

    if (number.longValue() == 1 || number.longValue() == 2) {
        newFormattedNumber = ::morphuntion::dialog::SpeakableString(newFormattedNumber.getSpeak(), newFormattedNumber.getSpeak());
    }
    return super::quantify(number, newFormattedNumber, semanticConcept);
}

morphuntion::dialog::SpeakableString HeCommonConceptFactory::quantifiedJoin(const morphuntion::dialog::SpeakableString& formattedNumber, const morphuntion::dialog::SpeakableString& nounPhrase, const ::std::u16string& /*measureWord*/, Plurality::Rule countType) const
{
    if (dontQuantify.find(nounPhrase.getPrint()) != dontQuantify.end()) {
        return nounPhrase;
    }
    if (countType == Plurality::Rule::ONE) {
        return nounPhrase + ::morphuntion::dialog::SpeakableString(u" ") + formattedNumber;
    }
    else {
        return formattedNumber + ::morphuntion::dialog::SpeakableString(u" ") + nounPhrase;
    }

}

} // namespace morphuntion::dialog::language
