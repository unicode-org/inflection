/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <morphuntion/dialog/SpeakableString.h>
#include <CoreFoundation/CFError.h>

MORPHUNTION_CTYPE(MDSemanticFeatureConcept)
/**
 * Returns {@code true} if a value exists for the given constraints
 */
MORPHUNTION_CAPI bool msfc_isExists(MDSemanticFeatureConcept* thisObject, CFErrorRef* error);
/**
 * Retrieves the value from the feature name.
 */
MORPHUNTION_CAPI CFStringRef msfc_createConstraintByNameCopy(const MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFErrorRef* error);
/**
 * Adds a constraint on the possible values to this concept.
 */
MORPHUNTION_CAPI void msfc_putConstraintByName(MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFStringRef featureValue, CFErrorRef* error);
/**
 * Clears a constraint on the possible values to this concept.
 */
MORPHUNTION_CAPI void msfc_clearConstraintByName(MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFErrorRef* error);
/**
 * Clear all constraints imposed on this SemanticFeatureConcept.
 */
MORPHUNTION_CAPI void msfc_reset(MDSemanticFeatureConcept* thisObject, CFErrorRef* error);
/**
 * Returns the String value of the specified feature for this concept, given its current constraints.
 * The featureName will be automatically converted to a SemanticFeature for lookup.
 */
MORPHUNTION_CAPI MDSpeakableString* msfc_createFeatureValueByNameCopy(const MDSemanticFeatureConcept* thisObject, CFStringRef featureName, CFErrorRef* error);
/**
 * Converts the concept to a Speakable String.
 *
 * @return Speakable String
 */
MORPHUNTION_CAPI MDSpeakableString* msfc_toSpeakableStringCopy(const MDSemanticFeatureConcept* thisObject, CFErrorRef* error);
/**
 * Converts the concept to a Speakable String depending on a dependency.
 *
 * @return Speakable String
 */
MORPHUNTION_CAPI MDSpeakableString* msfc_toSpeakableStringWithDependencyCopy(const MDSemanticFeatureConcept* thisObject, const MDSemanticFeatureConcept* referencedConcept, CFErrorRef* error);
/**
 * Clones the concept
 *
 * @return concept
 */
MORPHUNTION_CAPI MDSemanticFeatureConcept* msfc_clone(const MDSemanticFeatureConcept* thisObject, CFErrorRef* error);
/**
 * Destructor
 */
MORPHUNTION_CAPI void msfc_destroy(MDSemanticFeatureConcept* thisObject);
