/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <unicode/utypes.h>

/**
 * Represents a string with both a spoken and written representation
 */
INFLECTION_CTYPE(IDSpeakableString);
/**
 * Creates a CF string copy of the print string of the speakable string.
 *
 * @param thisObject The object to query
 * @param dest      A buffer for the result string. The result will be zero-terminated if
 *                  the buffer is large enough.
 * @param destCapacity The size of the buffer (number of UChars). If it is 0, then
 *                  dest may be NULL and the function will only return the length of the result
 *                  without writing any of the result string.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return The length of the result string. It may be greater than destCapacity. In that case,
 *         only some of the result was written to the destination buffer.
 */
INFLECTION_CAPI int32_t iss_getPrint(IDSpeakableString* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status);
/**
 * Creates a CF string copy of the speak string of the speakable string.
 *
 * @param thisObject The object to query
 * @param dest      A buffer for the result string. The result will be zero-terminated if
 *                  the buffer is large enough.
 * @param destCapacity The size of the buffer (number of UChars). If it is 0, then
 *                  dest may be NULL and the function will only return the length of the result
 *                  without writing any of the result string.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return The length of the result string. It may be greater than destCapacity. In that case,
 *         only some of the result was written to the destination buffer.
 */
INFLECTION_CAPI int32_t iss_getSpeak(IDSpeakableString* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status);
/**
 * Returns true when the print and speak values are equal.
 * @param thisObject The object to query
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI bool iss_speakEqualsPrint(IDSpeakableString* thisObject, UErrorCode* status);
/**
 * Checks that the print part contains a given substring.
 *
 * @param thisObject The object to query
 * @param str The substring to check for containment.
 * @param strLen The string length of str.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return true if the str is within the print value.
 */
INFLECTION_CAPI bool iss_contains(IDSpeakableString* thisObject, const char16_t* str, int32_t strLen, UErrorCode* status);
/**
 * Constructs a speakable string given the print part.
 *
 * @param print The expected print part for the speakable string
 * @param printLen The string length of print.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDSpeakableString* iss_create(const char16_t* print, int32_t printLen, UErrorCode* status);
/**
 * Constructs a speakable string given both the print and speak part.
 *
 * @param print The expected print part for the speakable string
 * @param printLen The string length of print.
 * @param speak The expected speak part for the speakable string
 * @param speakLen The string length of speak.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDSpeakableString* iss_createPrintSpeak(const char16_t* print, int32_t printLen, const char16_t* speak, int32_t speakLen, UErrorCode* status);
/**
 * Destructor.
 */
INFLECTION_CAPI void iss_destroy(IDSpeakableString* thisObject);
