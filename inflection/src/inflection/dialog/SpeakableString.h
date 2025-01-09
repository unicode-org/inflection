/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <CoreFoundation/CFString.h>
#include <unicode/utypes.h>

/**
 * Represents a string with both a spoken and written representation
 */
INFLECTION_CTYPE(IDSpeakableString);
/**
 * Creates a CF string copy of the print string of the speakable string.
 *
 * @param thisObject The object to query
 * @param error The provided error reference is allocated when the function failed.
 * @return CF string reference
 */
INFLECTION_CAPI CFStringRef iss_createPrintCopy(IDSpeakableString* thisObject, UErrorCode* status);
/**
 * Creates a CF string copy of the speak string of the speakable string.
 *
 * @param thisObject The object to query
 * @param error The provided error reference is allocated when the function failed.
 * @return CF string reference
 */
INFLECTION_CAPI CFStringRef iss_createSpeakCopy(IDSpeakableString* thisObject, UErrorCode* status);
/**
 * Returns true when the print and speak values are equal.
 * @param thisObject The object to query
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI bool iss_speakEqualsPrint(IDSpeakableString* thisObject, UErrorCode* status);
/**
 * Checks that the print part contains a given substring.
 *
 * @param thisObject The object to query
 * @param s The substring to check for containment.
 * @param error The provided error reference is allocated when the function failed.
 * @return  boolean
 */
INFLECTION_CAPI bool iss_contains(IDSpeakableString* thisObject, CFStringRef s, UErrorCode* status);
/**
 * Constructs a speakable string given the print part.
 *
 * @param print The expected print part for the speakable string
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI IDSpeakableString* iss_create(CFStringRef print, UErrorCode* status);
/**
 * Constructs a speakable string given both the print and speak part.
 *
 * @param print The expected print part for the speakable string
 * @param speak The expected speak part for the speakable string
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI IDSpeakableString* iss_createPrintSpeak(CFStringRef print, CFStringRef speak, UErrorCode* status);
/**
 * Destructor.
 */
INFLECTION_CAPI void iss_destroy(IDSpeakableString* thisObject);
