/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticFeatureConcept.h>

#include <inflection/dialog/PronounConcept.hpp>
#include <inflection/dialog/SemanticFeatureConceptBase.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/npc.hpp>
/**
 * Returns {@code true} if a value exists for the given constraints
 */
INFLECTION_CAPI bool
isfc_isExists(IDSemanticFeatureConcept* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (npc((::inflection::dialog::SemanticFeatureConceptBase*)thisObject)->isExists());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI CFStringRef
isfc_createConstraintByNameCopy(const IDSemanticFeatureConcept* thisObject, const char16_t* featureName, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            const auto thisObjectConcept = npc((const ::inflection::dialog::SemanticFeatureConceptBase*)thisObject);
            const auto model = thisObjectConcept->getModel();
            auto feature = npc(model)->getFeature(std::u16string_view(npc(featureName)));
            if (feature == nullptr) {
                throw ::inflection::exception::IllegalArgumentException(std::u16string(npc(featureName)) + u" is not a valid feature name for the SemanticFeatureModel");
            }
            auto result= thisObjectConcept->getConstraint(*feature);
            if (result != nullptr) {
                return inflection::util::TypeConversionUtils::to_CFString(*result);
            }
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

/**
 * Adds a constraint on the possible values to this concept.
 */
INFLECTION_CAPI void
isfc_putConstraintByName(IDSemanticFeatureConcept* thisObject, const char16_t* featureName, CFStringRef featureValue, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            (npc((::inflection::dialog::SemanticFeatureConceptBase*)thisObject)->putConstraintByName(npc(featureName), ::inflection::util::TypeConversionUtils::to_u16string(featureValue)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
}

/**
 * Clears a constraint on the possible values to this concept.
 */
INFLECTION_CAPI void
isfc_clearConstraintByName(IDSemanticFeatureConcept* thisObject, const char16_t* featureName, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            (npc((::inflection::dialog::SemanticFeatureConceptBase*)thisObject)->clearConstraintByName(npc(featureName)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
}

/**
 * Clear all constraints imposed on this SemanticFeatureConcept.
 */
INFLECTION_CAPI void
isfc_reset(IDSemanticFeatureConcept* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            (npc((::inflection::dialog::SemanticFeatureConceptBase*)thisObject)->reset());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
}

/**
 * Returns the String value of the specified feature for this concept, given its current constraints.
 * The featureName will be automatically converted to a SemanticFeature for lookup.
 */
INFLECTION_CAPI IDSpeakableString*
isfc_createFeatureValueByNameCopy(const IDSemanticFeatureConcept* thisObject, const char16_t* featureName, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*)(npc((const ::inflection::dialog::SemanticFeatureConceptBase*)thisObject)->getFeatureValueByName(npc(featureName)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString*
isfc_toSpeakableStringCopy(const IDSemanticFeatureConcept* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*)(npc((const ::inflection::dialog::SpeakableConcept*)thisObject)->toSpeakableString());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString*
isfc_toSpeakableStringWithDependencyCopy(const IDSemanticFeatureConcept* thisObject, const IDSemanticFeatureConcept* referencedConcept, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            auto thisConcept = reinterpret_cast<const ::inflection::dialog::SpeakableConcept*>(thisObject);
            if (dynamic_cast<const ::inflection::dialog::PronounConcept*>(thisConcept) != nullptr) {
                auto referencedConceptBase = reinterpret_cast<const ::inflection::dialog::SemanticFeatureConceptBase*>(referencedConcept);
                return (IDSpeakableString*)(npc(dynamic_cast<const ::inflection::dialog::PronounConcept*>(thisConcept))->toSpeakableString(*npc(referencedConceptBase)));
            }
            return (IDSpeakableString*)(npc(thisConcept)->toSpeakableString());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSemanticFeatureConcept*
isfc_clone(const IDSemanticFeatureConcept* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSemanticFeatureConcept*)(npc((const ::inflection::dialog::SemanticFeatureConceptBase*)thisObject)->clone());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

/**
 * Destructor
 */
INFLECTION_CAPI void
isfc_destroy(IDSemanticFeatureConcept* thisObject)
{
    delete (::inflection::dialog::SemanticFeatureConceptBase*)thisObject;
}

