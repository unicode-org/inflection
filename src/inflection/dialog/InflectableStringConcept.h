/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/SpeakableString.h>
#include <inflection/dialog/SemanticFeatureConcept.h>
#include <inflection/dialog/SemanticFeatureModel.h>
#include <inflection/dialog/DisplayValue.h>
/**
 * An object that provides a way to format a word with additional grammatical category values or semantic features of a word for a given language.
 */
INFLECTION_CTYPE(IDInflectableStringConcept)

/**
 * Converts the concept to a SemanticFeatureConcept
 *
 * @return SemanticFeatureConcept
 */
INFLECTION_CAPI IDSemanticFeatureConcept* iinf_toSemanticFeatureConcept(IDInflectableStringConcept* thisObject, UErrorCode* status);
/**
 * Converts the concept to a InflectableStringConcept
 *
 * @return InflectableStringConcept
 */
INFLECTION_CAPI IDInflectableStringConcept* iinf_toInflectableStringConcept(IDSemanticFeatureConcept* thisObject, UErrorCode* status);
/**
 * Constructs a concept given a semantic feature model and a speakable string
 *
 * @param model - The semantic feature model required to initialize the concept.
 * @param value - The speakable string to convert to a concept
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDInflectableStringConcept* iinf_create(const IDSemanticFeatureModel* model, const IDSpeakableString* value, UErrorCode* status);
/**
 * Constructs a concept given a semantic feature model and a speakable string
 *
 * @param model - The semantic feature model required to initialize the concept.
 * @param value - The speakable string to convert to a concept
 * @param defaultConstraints - The initial defaultConstraints to apply to the concept
 * @param defaultConstraintsLen - The number of semantic features and values provided
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDInflectableStringConcept* iinf_createWithDefaults(const IDSemanticFeatureModel* model, const IDSpeakableString* value, 
                                                                        const IDDisplayValue_Constraint* defaultConstraints, int32_t defaultConstraintsLen, UErrorCode* status);
/**
 * Destructor
 */
INFLECTION_CAPI void iinf_destroy(IDInflectableStringConcept* thisObject);
