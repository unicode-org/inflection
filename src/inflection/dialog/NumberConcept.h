/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/SpeakableString.h>
#include <unicode/utypes.h>
INFLECTION_CTYPE(IDNumberConcept)

/**
 * Format the number with words. The words are based on the language used to construct this object.
 * @param thisObject The object to format
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
INFLECTION_CAPI int32_t inum_asWords(const IDNumberConcept* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status);

/**
 * Format the number with words with the variant rule name. The words are based on the language used to construct this object.
 * @param thisObject The object to format
 * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp for details.
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
INFLECTION_CAPI int32_t inum_asWordsVariant(const IDNumberConcept* thisObject, const char16_t* variant, char16_t* dest, int32_t destCapacity, UErrorCode* status);

/**
 * Format the number with a printable and speakable form. The print part of the SpeakableString is the same as asDigits().
 * The speak part of the SpeakableString is the same as asWords().
 * @param thisObject The object to format
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDSpeakableString* inum_asSpokenWordsCopy(const IDNumberConcept* thisObject, UErrorCode* status);

/**
 * Format the number with a printable and speakable form. The print part of the SpeakableString is the same as asDigits().
 * The speak part of the SpeakableString is the same as asWords(variant).
 * @param thisObject The object to format
 * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp for details.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDSpeakableString* inum_asSpokenWordsVariantCopy(const IDNumberConcept* thisObject, const char16_t* variant, UErrorCode* status);

/**
 * Format the number with digits using the region locale. The speak part may contain words or digits for the given language
 * when the language is not the same as the region locale or the TTS system can not properly pronounce the printed form.
 * @param thisObject The object to format
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDSpeakableString* inum_asDigitsCopy(const IDNumberConcept* thisObject, UErrorCode* status);

/**
 * Format the number as ordinal digits using the language.
 * For example, the number would be formatted as 1st, 2nd and 3rd in English.
 * @see https://st.unicode.org/cldr-apps/numbers.jsp?type=OrdinalRules
 * @param thisObject The object to format
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
INFLECTION_CAPI int32_t inum_asOrdinalDigits(const IDNumberConcept* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status);

/**
 * Format the number as ordinal digits using the language with the requested variant.
 * For example, you can request the Spanish forms for 1º. and 1ª.
 * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp?type=OrdinalRules for details.
 * @param thisObject The object to format
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
INFLECTION_CAPI int32_t inum_asOrginalDigitsVariant(const IDNumberConcept* thisObject, const char16_t* variant, char16_t* dest, int32_t destCapacity, UErrorCode* status);

/**
 * Return a debuggable form of this NumberConcept.
 * @param thisObject The object to format
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
INFLECTION_CAPI int32_t inum_toString(const IDNumberConcept* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status);

/**
 * Construct a NumberConcept.
 * @param value a 64-bit value
 * @param language This locale is primarily used for the speak property of a SpeakableString.
 * @param regionLocale This locale is primarily used for the print property of a SpeakableString.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDNumberConcept* inum_createFromInt64(int64_t value, const char* language, const char* regionLocale, UErrorCode* status);

/**
 * Construct a NumberConcept.
 * @param value an IEEE 754 double value.
 * @param language This locale is primarily used for the speak property of a SpeakableString.
 * @param regionLocale This locale is primarily used for the print property of a SpeakableString.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDNumberConcept* inum_createFromDouble(double value, const char* language, const char* regionLocale, UErrorCode* status);

/**
 * Copy constructor
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI IDNumberConcept* inum_clone(const IDNumberConcept* original, UErrorCode* status);

/**
 * Destructor
 */
INFLECTION_CAPI void inum_destroy(IDNumberConcept *thisObject);
