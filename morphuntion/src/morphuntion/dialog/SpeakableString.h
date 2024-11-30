/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFString.h>

/**
 * Represents a string with both a spoken and written representation
 */
MORPHUNTION_CTYPE(MDSpeakableString);
/**
 * Creates a CF string copy of the print string of the speakable string.
 *
 * @param thisObject The object to query
 * @param error The provided error reference is allocated when the function failed.
 * @return CF string reference
 */
MORPHUNTION_CAPI CFStringRef mss_createPrintCopy(MDSpeakableString* thisObject, CFErrorRef* error);
/**
 * Creates a CF string copy of the speak string of the speakable string.
 *
 * @param thisObject The object to query
 * @param error The provided error reference is allocated when the function failed.
 * @return CF string reference
 */
MORPHUNTION_CAPI CFStringRef mss_createSpeakCopy(MDSpeakableString* thisObject, CFErrorRef* error);
/**
 * Returns true when the print and speak values are equal.
 * @param thisObject The object to query
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI bool mss_speakEqualsPrint(MDSpeakableString* thisObject, CFErrorRef* error);
/**
 * Checks that the print part contains a given substring.
 *
 * @param thisObject The object to query
 * @param s The substring to check for containment.
 * @param error The provided error reference is allocated when the function failed.
 * @return  boolean
 */
MORPHUNTION_CAPI bool mss_contains(MDSpeakableString* thisObject, CFStringRef s, CFErrorRef* error);
/**
 * Constructs a speakable string given the print part.
 *
 * @param print The expected print part for the speakable string
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDSpeakableString* mss_create(CFStringRef print, CFErrorRef* error);
/**
 * Constructs a speakable string given both the print and speak part.
 *
 * @param print The expected print part for the speakable string
 * @param speak The expected speak part for the speakable string
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDSpeakableString* mss_createPrintSpeak(CFStringRef print, CFStringRef speak, CFErrorRef* error);
/**
 * Destructor.
 */
MORPHUNTION_CAPI void mss_destroy(MDSpeakableString* thisObject);
