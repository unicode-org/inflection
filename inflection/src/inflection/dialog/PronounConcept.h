/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/DisplayValue.h>
#include <inflection/dialog/SpeakableString.h>
#include <inflection/dialog/SemanticFeatureModel.h>
#include <inflection/dialog/SemanticFeatureConcept.h>
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
 * @param defaultDisplayData An array of IDDisplayValue objects. The contents are copied and not referenced after this call.
 * @param defaultDisplayDataLen The length of defaultDisplayData.
 * @param defaultConstraints The constraints to consider when a given constraint is undefined. The contents are copied and not referenced after this call.
 * @param defaultConstraintsLen The length of defaultConstraints.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDPronounConcept* ipron_createWithCustom(const IDSemanticFeatureModel* model,
                                                         const IDDisplayValue* defaultDisplayData,
                                                         int32_t defaultDisplayDataLen,
                                                         const IDDisplayValue_Constraint* defaultConstraints,
                                                         int32_t defaultConstraintsLen,
                                                         UErrorCode* status);
/**
 * Constructs a new PronounConcept with the default constraints. If you know the gender or person grammatical category
 * values of the desired pronoun, it's recommended to provide them with defaultConstraints.
 * A SemanticFeatureModel is required, as it will enforce the locale and provide the known constraints for the given language.
 * @param model The SemanticFeatureModel for the language
 * @param defaultConstraints The constraints to consider when a given constraint is undefined.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDPronounConcept* ipron_createWithDefaults(const IDSemanticFeatureModel* model,
                                                           const IDDisplayValue_Constraint* defaultConstraints,
                                                           int32_t defaultConstraintsLen,
                                                           UErrorCode* status);
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
 * @param initialPronounLen The string length of initialPronoun.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @throws IllegalArgumentException Thrown when the pronoun is not a known pronoun for the given language.
 */
INFLECTION_CAPI IDPronounConcept* ipron_createFromInitialPronoun(const IDSemanticFeatureModel* model, const char16_t* initialPronoun, int32_t initialPronounLen, UErrorCode* status);
/**
 * Destructor
 */
INFLECTION_CAPI void ipron_destroy(IDPronounConcept* thisObject);
/**
 * Returns true if the given constraints match a custom pronoun specified in ipron_createWithCustom.
 */
INFLECTION_CAPI bool ipron_isCustomMatch(const IDPronounConcept* thisObject, UErrorCode* status);
