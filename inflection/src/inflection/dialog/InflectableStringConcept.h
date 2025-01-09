/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/SpeakableString.h>
#include <inflection/dialog/SemanticFeatureConcept.h>
#include <inflection/dialog/SemanticFeatureModel.h>
#include <CoreFoundation/CFString.h>

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
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI IDInflectableStringConcept* iinf_create(const IDSemanticFeatureModel* model, const IDSpeakableString* value, UErrorCode* status);
/**
 * Destructor
 */
INFLECTION_CAPI void iinf_destroy(IDInflectableStringConcept* thisObject);
