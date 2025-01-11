/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>

#include <inflection/dialog/NumberConcept.h>
#include <inflection/dialog/SemanticFeatureModel.h>
#include <inflection/dialog/SemanticFeatureConcept.h>
#include <inflection/dialog/SpeakableString.h>

/**
 * @brief Facilitates the creation of _Concepts_
 * @details Concepts can generally be defined as a way to lexically represent real-world or linguistic ideas.
 * These ideas can vary greatly in scope, and range from the representation of quoting a string to getting a quantity
 * with a number and unit into grammatical agreement.
 *
 * @see ilccfp_getCommonConceptFactory for retrieving a default implementation.
 */
INFLECTION_CTYPE(IDCommonConceptFactory)

/**
 * Returns a pointer to the inheriting ConceptFactory's SemanticFeatureModel.
 *
 * @param thisObject The object to query
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return The underlying language's SemanticFeatureModel
 */
INFLECTION_CAPI const IDSemanticFeatureModel* iccf_getSemanticFeatureModel(const IDCommonConceptFactory *thisObject, UErrorCode* status);

/**
 * Returns this ConceptFactory's underlying language.
 *
 * @param thisObject The object to query
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return The language.
 */
INFLECTION_CAPI const char* iccf_getLanguage(const IDCommonConceptFactory* thisObject, UErrorCode* status);

/**
 * Returns a SpeakableString based on an input SpeakableString and with the
 * print component surrounded with the locale's quotation punctuation, in English this is represented with
 * quotation marks: <code>""</code>. Another example is in Swiss German dialects this is represented with
 * <code>«»</code>.
 *
 * @param thisObject The CommonConceptFactory used for formatting <code>str</code>
 * @param str The input SpeakableString to add quotation punctuation to.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return The quoted SpeakableString.
 */
INFLECTION_CAPI IDSpeakableString* iccf_quote(const IDCommonConceptFactory* thisObject, const IDSpeakableString* str, UErrorCode* status);

/**
 * Returns a new SpeakableString based on a NumberConcept and SemanticConcept.
 *
 * Usage for this would be to automatically determine plurality rules for words. This is especially
 * useful for words that have multiple rules based on amounts, such as Slavic languages, in which the genetive
 * plural form has a different inflection.
 *
 * For example in Russian:<br/>
 * 0 years → 0 лет<br/>
 * 1 year → 1 год<br/>
 * 4 years → 4 года<br/>
 * 5 years → 5 лет<br/>
 * 100 years → 100 лет
 *
 * The returned object is owned by the caller and must be deleted by the caller.
 *
 * @param thisObject The CommonConceptFactory used for constructing a SpeakableString
 * @param number An input NumberConcept to represent the amount.
 * @param semanticConcept An input SemanticConcept to represent the word to be inflected based on
 * the amount.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return A new SpeakableString.
 */
INFLECTION_CAPI IDSpeakableString* iccf_quantifyCopy(const IDCommonConceptFactory* thisObject, const IDNumberConcept* number, const IDSemanticFeatureConcept* semanticConcept, UErrorCode* status);

/**
 * Returns a new SpeakableString based on a NumberConcept, format number, and SemanticConcept.
 *
 * The returned object is owned by the caller and must be deleted by the caller.
 *
 * @param thisObject The CommonConceptFactory used for constructing a SpeakableString
 * @param number An input NumberConcept to represent the amount.
 * @param formattedNumber If the default chosen cardinal number is wrong for the context of the noun, you can format a specific form of the number.
 * @param semanticConcept An input SemanticConcept to represent the word to be inflected based on
 * the amount.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return A new SpeakableString.
 */
INFLECTION_CAPI IDSpeakableString* iccf_quantifyFormattedCopy(const IDCommonConceptFactory* thisObject, const IDNumberConcept* number, const IDSpeakableString* formattedNumber, const IDSemanticFeatureConcept* semanticConcept, UErrorCode* status);
