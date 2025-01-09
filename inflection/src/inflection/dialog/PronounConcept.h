/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/SpeakableString.h>
#include <inflection/dialog/SemanticFeatureModel.h>
#include <inflection/dialog/SemanticFeatureConcept.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFString.h>
#include <unicode/utypes.h>

INFLECTION_CTYPE(IDPronounConcept)
/**
 * Converts the concept to a SemanticFeatureConcept
 *
 * @return SemanticFeatureConcept
 */
INFLECTION_CAPI IDSemanticFeatureConcept* ipron_toSemanticFeatureConcept(IDPronounConcept* thisObject, UErrorCode* status);
/**
 * Converts the concept to a PronounConcept
 *
 * @return PronounConcept
 */
INFLECTION_CAPI IDPronounConcept* ipron_toPronounConcept(IDSemanticFeatureConcept* thisObject, UErrorCode* status);
/**
 * Constructs a new PronounConcept based on dislay values with custom pronouns. Any unspecified pronouns will use the language's default pronouns.
 * @param model The SemanticFeatureModel for the language
 * @param defaultDisplayData An array of IDSemanticFeatureModel_DisplayValue objects
 * @param defaultConstraints The constraints to consider when a given constraint is undefined.
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI IDPronounConcept* ipron_createWithCustom(const IDSemanticFeatureModel* model, CFArrayRef defaultDisplayData, CFDictionaryRef defaultConstraints, UErrorCode* status);
/**
 * Constructs a new PronounConcept with the default constraints. If you know the gender or person grammatical category
 * values of the desired pronoun, it's recommended to provide them with defaultConstraints.
 * A SemanticFeatureModel is required, as it will enforce the locale and provide the known constraints for the given language.
 * @param model The SemanticFeatureModel for the language
 * @param defaultConstraints The constraints to consider when a given constraint is undefined.
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI IDPronounConcept* ipron_createWithDefaults(const IDSemanticFeatureModel* model, CFDictionaryRef defaultConstraints, UErrorCode* status);
/**
 * Constructs a new PronounConcept with the default constraints based on an existing pronoun from the language.
 * For example, "I" will provide the default constraints "first", "singular" and "subjective" for English.
 * If "her" was provided for initialPronoun in English, then the default constraints will be "third", "singular", "feminine", and "objective".
 * If "her" was the in English, and then the constraints "possessive" and "independent" were added, then formatting
 * this PronounConcept with toSpeakableString() will provide "hers". The "possessive" constraint would override
 * the "objective" constraint.
 *
 * A SemanticFeatureModel is required, as it will enforce the locale and provide the known constraints for the given language.
 * @param model The SemanticFeatureModel for the language
 * @param initialPronoun The language specific pronoun to start inflecting the pronoun.
 * @param error The provided error reference is allocated when the function failed.
 * @throws IllegalArgumentException Thrown when the pronoun is not a known pronoun for the given language.
 */
INFLECTION_CAPI IDPronounConcept* ipron_createFromInitialPronoun(const IDSemanticFeatureModel* model, CFStringRef initialPronoun, UErrorCode* status);
/**
 * Destructor
 */
INFLECTION_CAPI void ipron_destroy(IDPronounConcept* thisObject);
/**
 * Returns true if the given constraints match a custom pronoun specified in ipron_createWithCustom.
 */
INFLECTION_CAPI bool ipron_isCustomMatch(const IDPronounConcept* thisObject, UErrorCode* status);
