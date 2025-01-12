/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <unicode/utypes.h>

/**
 * @brief Provides an interface to read Inflection's lexical dictionaries.
 */
INFLECTION_CTYPE(IDDictionaryMetaData)

/**
 * Get all of the binary types of a word or phrase. The name for each bit can be retrieved by getPropertyName().
 * @param thisObject The object to query
 * @param result - Pointer to the int64_t variable where the result would be stored
 * @param word - Word for which properties are requested
 * @param wordLen - The length of the word for which properties are requested
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return Pointer to the bitmask of all the properties for that word in the dictionary and null is returned if there is an error or the word is unknown.
 */
INFLECTION_CAPI int64_t* iddmd_getCombinedBinaryType(const IDDictionaryMetaData* thisObject, int64_t *result, const char16_t* word, int32_t wordLen, UErrorCode* status);

/**
 * Given a single bit turned on, return the name of the property.
 * @param thisObject The object to query
 * @param singleProperty - Property bit mask
 * @param dest name of the property, empty if no bits are set
 * @param destCapacity
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return The length of the result string. It may be greater than destCapacity. In that case,
 *         only some of the result was written to the destination buffer.
 */
INFLECTION_CAPI int32_t iddmd_getPropertyName(const IDDictionaryMetaData* thisObject, int64_t singleProperty, char16_t* dest, int32_t destCapacity, UErrorCode* status);

/**
 * Is the provided word a known word in the dictionary.
 * @param thisObject The object to query
 * @param word Word to search in the dictionary
 * @param wordLen String length of word
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return True if word is found in the dictionary false otherwise
 */
INFLECTION_CAPI bool iddmd_isKnownWord(const IDDictionaryMetaData* thisObject, const char16_t* word, int32_t wordLen, UErrorCode* status);

/**
 * Retrieve all of the property names and return set the provided return value.
 * @param thisObject The object to query
 * @param properties List of all property names
 * @param propertiesLength The length of property names
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return Bitmask representing the requested properties.
 */
INFLECTION_CAPI int64_t iddmd_getBinaryProperties(const IDDictionaryMetaData* thisObject, const char16_t* const properties[], int32_t propertiesLength, UErrorCode* status);

/**
 * Create a dictionary metadata object for the supplied locale
 * @param locale - locale of the dictionary
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return Dictionary metadata object for the given locale. Pointer owned by Inflection dictionary cache, don't memory manage it
 */
INFLECTION_CAPI const IDDictionaryMetaData* iddmd_getDictionary(const char* locale, UErrorCode* status);
