/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SemanticFeatureConcept.h>

#include <morphuntion/dialog/PronounConcept.hpp>
#include <morphuntion/dialog/SemanticFeatureConceptBase.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/exception/IllegalArgumentException.hpp>
#include <morphuntion/npc.hpp>
/**
 * Returns {@code true} if a value exists for the given constraints
 */
MORPHUNTION_CAPI bool
msfc_isExists(MDSemanticFeatureConcept* thisObject, CFErrorRef* error)
{
    try {
        return (npc((::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject)->isExists());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return false;
}

MORPHUNTION_CAPI CFStringRef
msfc_createConstraintByNameCopy(const MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFErrorRef* error)
{
    try {
        const auto thisObjectConcept = npc((const ::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject);
        auto constraint(::morphuntion::util::CFUtils::to_u16string(featureName));
        const auto model = thisObjectConcept->getModel();
        auto feature = npc(model)->getFeature(constraint);
        if (feature == nullptr) {
            throw ::morphuntion::exception::IllegalArgumentException(constraint + u" is not a valid feature name for the SemanticFeatureModel");
        }
        auto result= thisObjectConcept->getConstraint(*feature);
        if (result != nullptr) {
            return morphuntion::util::CFUtils::to_CFString(*result);
        }
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

/**
 * Adds a constraint on the possible values to this concept.
 */
MORPHUNTION_CAPI void
msfc_putConstraintByName(MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFStringRef featureValue, CFErrorRef* error)
{
    try {
        (npc((::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject)->putConstraintByName(::morphuntion::util::CFUtils::to_u16string(featureName),::morphuntion::util::CFUtils::to_u16string(featureValue)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
}

/**
 * Clears a constraint on the possible values to this concept.
 */
MORPHUNTION_CAPI void
msfc_clearConstraintByName(MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFErrorRef* error)
{
    try {
        (npc((::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject)->clearConstraintByName(::morphuntion::util::CFUtils::to_u16string(featureName)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
}

/**
 * Clear all constraints imposed on this SemanticFeatureConcept.
 */
MORPHUNTION_CAPI void
msfc_reset(MDSemanticFeatureConcept* thisObject, CFErrorRef* error)
{
    try {
        (npc((::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject)->reset());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
}

/**
 * Returns the String value of the specified feature for this concept, given its current constraints.
 * The featureName will be automatically converted to a SemanticFeature for lookup.
 */
MORPHUNTION_CAPI MDSpeakableString*
msfc_createFeatureValueByNameCopy(const MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFErrorRef* error)
{
    try {
        return (MDSpeakableString*)(npc((const ::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject)->getFeatureValueByName(::morphuntion::util::CFUtils::to_u16string(featureName)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString*
msfc_toSpeakableStringCopy(const MDSemanticFeatureConcept* thisObject, CFErrorRef* error)
{
    try {
        return (MDSpeakableString*)(npc((const ::morphuntion::dialog::SpeakableConcept*)thisObject)->toSpeakableString());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString*
msfc_toSpeakableStringWithDependencyCopy(const MDSemanticFeatureConcept* thisObject, const MDSemanticFeatureConcept* referencedConcept, CFErrorRef* error)
{
    try {
        auto thisConcept = reinterpret_cast<const ::morphuntion::dialog::SpeakableConcept*>(thisObject);
        if (dynamic_cast<const ::morphuntion::dialog::PronounConcept*>(thisConcept) != nullptr) {
            auto referencedConceptBase = reinterpret_cast<const ::morphuntion::dialog::SemanticFeatureConceptBase*>(referencedConcept);
            return (MDSpeakableString*)(npc(dynamic_cast<const ::morphuntion::dialog::PronounConcept*>(thisConcept))->toSpeakableString(*npc(referencedConceptBase)));
        }
        return (MDSpeakableString*)(npc(thisConcept)->toSpeakableString());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSemanticFeatureConcept*
msfc_clone(const MDSemanticFeatureConcept* thisObject, CFErrorRef* error)
{
    try {
        return (MDSemanticFeatureConcept*)(npc((const ::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject)->clone());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

/**
 * Destructor
 */
MORPHUNTION_CAPI void
msfc_destroy(MDSemanticFeatureConcept* thisObject)
{
    delete (::morphuntion::dialog::SemanticFeatureConceptBase*)thisObject;
}

