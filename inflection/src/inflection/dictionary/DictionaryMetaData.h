/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFArray.h>
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
 * @param error The provided error reference is allocated when the function failed.
 * @return Pointer to the bitmask of all the properties for that word in the dictionary and null is returned if there is an error or the word is unknown.
 */
INFLECTION_CAPI int64_t* iddmd_getCombinedBinaryType(const IDDictionaryMetaData* thisObject, int64_t *result, CFStringRef word, UErrorCode* status);

/**
 * Turns all of the bits in the binary properties into string based property names.
 * @param thisObject The object to query
 * @param binaryProperties - set of properties represented as a bitmask
 * @param error The provided error reference is allocated when the function failed.
 * @return list of all property names, elements are automatically released on releasing the array
 */
INFLECTION_CAPI CFArrayRef iddmd_createPropertyNames(const IDDictionaryMetaData* thisObject, int64_t binaryProperties, UErrorCode* status);

/**
 * Given a single bit turned on, return the name of the property.
 * @param thisObject The object to query
 * @param singleProperty - Property bit mask
 * @param error The provided error reference is allocated when the function failed.
 * @return name of the property, empty if exactly single bit is not set
 */
INFLECTION_CAPI CFStringRef iddmd_createPropertyName(const IDDictionaryMetaData* thisObject, int64_t singleProperty, UErrorCode* status);

/**
 * Is the provided word a known word in the dictionary.
 * @param thisObject The object to query
 * @param word - Word to search in the dictionary
 * @param error The provided error reference is allocated when the function failed.
 * @return True if word is found in the dictionary false otherwise
 */
INFLECTION_CAPI bool iddmd_isKnownWord(const IDDictionaryMetaData* thisObject, CFStringRef word, UErrorCode* status);

/**
 * Retrieve all of the property names and return set the provided return value.
 * @param thisObject The object to query
 * @param properties List of all property names
 * @param error The provided error reference is allocated when the function failed.
 * @return Bitmask representing the requested properties.
 */
INFLECTION_CAPI int64_t iddmd_getBinaryProperties(const IDDictionaryMetaData* thisObject, CFArrayRef properties, UErrorCode* status);

/**
 * Create a dictionary metadata object for the supplied locale
 * @param locale - locale of the dictionary
 * @param error The provided error reference is allocated when the function failed.
 * @return Dictionary metadata object for the given locale. Pointer owned by Inflection dictionary cache, don't memory manage it
 */
INFLECTION_CAPI const IDDictionaryMetaData* iddmd_getDictionary(const char* locale, UErrorCode* status);
