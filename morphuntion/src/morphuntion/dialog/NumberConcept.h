/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <morphuntion/dialog/SpeakableString.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFLocale.h>
#include <CoreFoundation/CFString.h>
MORPHUNTION_CTYPE(MDNumberConcept)

/**
 * Format the number with words. The words are based on the language used to construct this object.
 */
MORPHUNTION_CAPI CFStringRef mnum_asWordsCopy(const MDNumberConcept* thisObject, CFErrorRef* error);

/**
 * Format the number with words with the variant rule name. The words are based on the language used to construct this object.
 * @param thisObject The object to format
 * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp for details.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI CFStringRef mnum_asWordsVariantCopy(const MDNumberConcept* thisObject, CFStringRef variant, CFErrorRef* error);

/**
 * Format the number with a printable and speakable form. The print part of the SpeakableString is the same as asDigits().
 * The speak part of the SpeakableString is the same as asWords().
 * @param thisObject The object to format
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDSpeakableString* mnum_asSpokenWordsCopy(const MDNumberConcept* thisObject, CFErrorRef* error);

/**
 * Format the number with a printable and speakable form. The print part of the SpeakableString is the same as asDigits().
 * The speak part of the SpeakableString is the same as asWords(variant).
 * @param thisObject The object to format
 * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp for details.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDSpeakableString* mnum_asSpokenWordsVariantCopy(const MDNumberConcept* thisObject, CFStringRef variant, CFErrorRef* error);

/**
 * Format the number with digits using the region locale. The speak part may contain words or digits for the given language
 * when the language is not the same as the region locale or the TTS system can not properly pronounce the printed form.
 * @param thisObject The object to format
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDSpeakableString* mnum_asDigitsCopy(const MDNumberConcept* thisObject, CFErrorRef* error);

/**
 * Format the number as ordinal digits using the language.
 * For example, the number would be formatted as 1st, 2nd and 3rd in English.
 * @see https://st.unicode.org/cldr-apps/numbers.jsp?type=OrdinalRules
 * @param thisObject The object to format
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI CFStringRef mnum_asOrdinalDigitsCopy(const MDNumberConcept* thisObject, CFErrorRef* error);

/**
 * Format the number as ordinal digits using the language with the requested variant.
 * For example, you can request the Spanish forms for 1º. and 1ª.
 * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp?type=OrdinalRules for details.
 * @param thisObject The object to format
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI CFStringRef mnum_asOrginalDigitsVariantCopy(const MDNumberConcept* thisObject, CFStringRef variant, CFErrorRef* error);

/**
 * Return a debuggable form of this NumberConcept.
 * @param thisObject The object to format
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI CFStringRef mnum_toStringCopy(const MDNumberConcept* thisObject, CFErrorRef* error);

/**
 * Construct a NumberConcept.
 * @param value a 64-bit value
 * @param language This locale is primarily used for the speak property of a SpeakableString.
 * @param regionLocale This locale is primarily used for the print property of a SpeakableString.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDNumberConcept* mnum_createFromInt64(int64_t value, CFLocaleRef language, CFLocaleRef regionLocale, CFErrorRef* error);

/**
 * Construct a NumberConcept.
 * @param value an IEEE 754 double value.
 * @param language This locale is primarily used for the speak property of a SpeakableString.
 * @param regionLocale This locale is primarily used for the print property of a SpeakableString.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDNumberConcept* mnum_createFromDouble(double value, CFLocaleRef language, CFLocaleRef regionLocale, CFErrorRef* error);

/**
 * Copy constructor
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDNumberConcept* mnum_clone(const MDNumberConcept* original, CFErrorRef* error);

/**
 * Destructor
 */
MORPHUNTION_CAPI void mnum_destroy(MDNumberConcept *thisObject);
