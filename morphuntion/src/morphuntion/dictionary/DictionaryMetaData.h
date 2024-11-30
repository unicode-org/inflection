/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFError.h>

/**
 * @brief Provides an interface to read Morphuntion's lexical dictionaries.
 */
MORPHUNTION_CTYPE(MDDictionaryMetaData)

/**
 * Get all of the binary types of a word or phrase. The name for each bit can be retrieved by getPropertyName().
 * @param thisObject The object to query
 * @param result - Pointer to the int64_t variable where the result would be stored
 * @param word - Word for which properties are requested
 * @param error The provided error reference is allocated when the function failed.
 * @return Pointer to the bitmask of all the properties for that word in the dictionary and null is returned if there is an error or the word is unknown.
 */
MORPHUNTION_CAPI int64_t* mddmd_getCombinedBinaryType(const MDDictionaryMetaData* thisObject, int64_t *result, CFStringRef word, CFErrorRef* error);

/**
 * Turns all of the bits in the binary properties into string based property names.
 * @param thisObject The object to query
 * @param binaryProperties - set of properties represented as a bitmask
 * @param error The provided error reference is allocated when the function failed.
 * @return list of all property names, elements are automatically released on releasing the array
 */
MORPHUNTION_CAPI CFArrayRef mddmd_createPropertyNames(const MDDictionaryMetaData* thisObject, int64_t binaryProperties, CFErrorRef* error);

/**
 * Given a single bit turned on, return the name of the property.
 * @param thisObject The object to query
 * @param singleProperty - Property bit mask
 * @param error The provided error reference is allocated when the function failed.
 * @return name of the property, empty if exactly single bit is not set
 */
MORPHUNTION_CAPI CFStringRef mddmd_createPropertyName(const MDDictionaryMetaData* thisObject, int64_t singleProperty, CFErrorRef* error);

/**
 * Is the provided word a known word in the dictionary.
 * @param thisObject The object to query
 * @param word - Word to search in the dictionary
 * @param error The provided error reference is allocated when the function failed.
 * @return True if word is found in the dictionary false otherwise
 */
MORPHUNTION_CAPI bool mddmd_isKnownWord(const MDDictionaryMetaData* thisObject, CFStringRef word, CFErrorRef* error);

/**
 * Retrieve all of the property names and return set the provided return value.
 * @param thisObject The object to query
 * @param properties List of all property names
 * @param error The provided error reference is allocated when the function failed.
 * @return Bitmask representing the requested properties.
 */
MORPHUNTION_CAPI int64_t mddmd_getBinaryProperties(const MDDictionaryMetaData* thisObject, CFArrayRef properties, CFErrorRef* error);

/**
 * Create a dictionary metadata object for the supplied locale
 * @param locale - locale of the dictionary
 * @param error The provided error reference is allocated when the function failed.
 * @return Dictionary metadata object for the given locale. Pointer owned by Morphuntion dictionary cache, don't memory manage it
 */
MORPHUNTION_CAPI const MDDictionaryMetaData* mddmd_getDictionary(CFLocaleRef locale, CFErrorRef* error);
