/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <morphuntion/dialog/SpeakableString.h>
#include <morphuntion/dialog/SemanticFeatureModel.h>
#include <morphuntion/dialog/SemanticFeatureConcept.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFString.h>

MORPHUNTION_CTYPE(MDPronounConcept)
/**
 * Converts the concept to a SemanticFeatureConcept
 *
 * @return SemanticFeatureConcept
 */
MORPHUNTION_CAPI MDSemanticFeatureConcept* mpron_toSemanticFeatureConcept(MDPronounConcept* thisObject, CFErrorRef* error);
/**
 * Converts the concept to a PronounConcept
 *
 * @return PronounConcept
 */
MORPHUNTION_CAPI MDPronounConcept* mpron_toPronounConcept(MDSemanticFeatureConcept* thisObject, CFErrorRef* error);
/**
 * Constructs a new PronounConcept based on dislay values with custom pronouns. Any unspecified pronouns will use the language's default pronouns.
 * @param model The SemanticFeatureModel for the language
 * @param defaultDisplayData An array of MDSemanticFeatureModel_DisplayValue objects
 * @param defaultConstraints The constraints to consider when a given constraint is undefined.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDPronounConcept* mpron_createWithCustom(const MDSemanticFeatureModel* model, CFArrayRef defaultDisplayData, CFDictionaryRef defaultConstraints, CFErrorRef* error);
/**
 * Constructs a new PronounConcept with the default constraints. If you know the gender or person grammatical category
 * values of the desired pronoun, it's recommended to provide them with defaultConstraints.
 * A SemanticFeatureModel is required, as it will enforce the locale and provide the known constraints for the given language.
 * @param model The SemanticFeatureModel for the language
 * @param defaultConstraints The constraints to consider when a given constraint is undefined.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDPronounConcept* mpron_createWithDefaults(const MDSemanticFeatureModel* model, CFDictionaryRef defaultConstraints, CFErrorRef* error);
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
MORPHUNTION_CAPI MDPronounConcept* mpron_createFromInitialPronoun(const MDSemanticFeatureModel* model, CFStringRef initialPronoun, CFErrorRef* error);
/**
 * Destructor
 */
MORPHUNTION_CAPI void mpron_destroy(MDPronounConcept* thisObject);
/**
 * Returns true if the given constraints match a custom pronoun specified in mpron_createWithCustom.
 */
MORPHUNTION_CAPI bool mpron_isCustomMatch(const MDPronounConcept* thisObject, CFErrorRef* error);
