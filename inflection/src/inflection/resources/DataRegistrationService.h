/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <CoreFoundation/CFString.h>
#include <unicode/utypes.h>
/**
 * Registers a path on the filesystem for Inflection to use as the backing data for a given locale.
 *
 * @param locale The locale to register the data for.
 * @param path The path on the filesystem (relative or absolute).
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI void idr_registerDataPathForLocale(const char* locale, CFStringRef path, UErrorCode* status);
/**
 * Returns the filesystem path that was previously registered for a given locale.
 *
 * @param locale The locale.
 * @param error The provided error reference is allocated when the function failed.
 * @return The path exactly as previously registered.
 */
INFLECTION_CAPI CFStringRef idr_getDataPathForLocaleCopy(const char* locale, UErrorCode* status);
