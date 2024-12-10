/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <morphuntion/dialog/SpeakableString.h>
#include <morphuntion/dialog/SemanticFeatureConcept.h>
#include <morphuntion/dialog/SemanticFeatureModel.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFString.h>

/**
 * An object that provides a way to format a word with additional grammatical category values or semantic features of a word for a given language.
 */
MORPHUNTION_CTYPE(MDInflectableStringConcept)

/**
 * Converts the concept to a SemanticFeatureConcept
 *
 * @return SemanticFeatureConcept
 */
MORPHUNTION_CAPI MDSemanticFeatureConcept* minf_toSemanticFeatureConcept(MDInflectableStringConcept* thisObject, CFErrorRef* error);
/**
 * Converts the concept to a InflectableStringConcept
 *
 * @return InflectableStringConcept
 */
MORPHUNTION_CAPI MDInflectableStringConcept* minf_toInflectableStringConcept(MDSemanticFeatureConcept* thisObject, CFErrorRef* error);
/**
 * Converts the concept to a Speakable String
 *
 * @return Speakable String
 */
MORPHUNTION_CAPI MDSpeakableString* minf_toSpeakableString(MDInflectableStringConcept* thisObject, CFErrorRef* error);
/**
 * Clones the concept
 *
 * @return concept
 */
MORPHUNTION_CAPI MDInflectableStringConcept* minf_clone(MDInflectableStringConcept* thisObject, CFErrorRef* error);
/**
 * Constructs a concept given a semantic feature model and a speakable string
 *
 * @param model - The semantic feature model required to initialize the concept.
 * @param value - The speakable string to convert to a concept
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDInflectableStringConcept* minf_create(const MDSemanticFeatureModel* model, const MDSpeakableString* value, CFErrorRef* error);
/**
 * Destructor
 */
MORPHUNTION_CAPI void minf_destroy(MDInflectableStringConcept* thisObject);

/**
 * Test whether the string is inflectable without making a guess
 * Returns true if the string is inflectable without a guess
 */
MORPHUNTION_CAPI bool minf_isExists(MDInflectableStringConcept* thisObject, CFErrorRef* error);
MORPHUNTION_CAPI void minf_putConstraintByName(MDInflectableStringConcept* thisObject, CFStringRef featureName, CFStringRef featureValue, CFErrorRef* error);
MORPHUNTION_CAPI void minf_clearConstraintByName(MDInflectableStringConcept* thisObject, CFStringRef featureName, CFErrorRef* error);
/**
 * Clear all constraints set with minf_putConstraintByName.
 */
MORPHUNTION_CAPI void minf_reset(MDInflectableStringConcept* thisObject, CFErrorRef* error);
MORPHUNTION_CAPI MDSpeakableString* minf_getFeatureValueByNameCopy(MDInflectableStringConcept* thisObject, CFStringRef featureName, CFErrorRef* error);
