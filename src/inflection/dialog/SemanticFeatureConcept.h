/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/SpeakableString.h>
#include <unicode/utypes.h>

INFLECTION_CTYPE(IDSemanticFeatureConcept)
/**
 * Returns {@code true} if a value exists for the given constraints
 */
INFLECTION_CAPI bool isfc_isExists(IDSemanticFeatureConcept* thisObject, UErrorCode* status);
/**
 * Retrieves the value from the feature name.
 */
INFLECTION_CAPI int32_t isfc_getConstraintByName(const IDSemanticFeatureConcept* thisObject, const char16_t* featureName, char16_t* dest, int32_t destCapacity, UErrorCode* status);
/**
 * Adds a constraint on the possible values to this concept.
 */
INFLECTION_CAPI void isfc_putConstraintByName(IDSemanticFeatureConcept* thisObject, const char16_t* featureName, const char16_t* featureValue, int32_t featureValueLen, UErrorCode* status);
/**
 * Clears a constraint on the possible values to this concept.
 */
INFLECTION_CAPI void isfc_clearConstraintByName(IDSemanticFeatureConcept* thisObject, const char16_t* featureName, UErrorCode* status);
/**
 * Clear all constraints imposed on this SemanticFeatureConcept.
 */
INFLECTION_CAPI void isfc_reset(IDSemanticFeatureConcept* thisObject, UErrorCode* status);
/**
 * Returns the String value of the specified feature for this concept, given its current constraints.
 * The featureName will be automatically converted to a SemanticFeature for lookup.
 */
INFLECTION_CAPI IDSpeakableString* isfc_createFeatureValueByNameCopy(const IDSemanticFeatureConcept* thisObject, const char16_t* featureName, UErrorCode* status);
/**
 * Converts the concept to a Speakable String.
 *
 * @return Speakable String
 */
INFLECTION_CAPI IDSpeakableString* isfc_toSpeakableStringCopy(const IDSemanticFeatureConcept* thisObject, UErrorCode* status);
/**
 * Converts the concept to a Speakable String depending on a dependency.
 *
 * @return Speakable String
 */
INFLECTION_CAPI IDSpeakableString* isfc_toSpeakableStringWithDependencyCopy(const IDSemanticFeatureConcept* thisObject, const IDSemanticFeatureConcept* referencedConcept, UErrorCode* status);
/**
 * Clones the concept
 *
 * @return concept
 */
INFLECTION_CAPI IDSemanticFeatureConcept* isfc_clone(const IDSemanticFeatureConcept* thisObject, UErrorCode* status);
/**
 * Destructor
 */
INFLECTION_CAPI void isfc_destroy(IDSemanticFeatureConcept* thisObject);
