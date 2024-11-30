/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFString.h>
/**
 * Registers a path on the filesystem for Morphuntion to use as the backing data for a given locale.
 *
 * @param locale The locale to register the data for.
 * @param path The path on the filesystem (relative or absolute).
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI void mdr_registerDataPathForLocale(CFLocaleRef locale, CFStringRef path, CFErrorRef* error);
/**
 * Returns the filesystem path that was previously registered for a given locale.
 *
 * @param locale The locale.
 * @param error The provided error reference is allocated when the function failed.
 * @return The path exactly as previously registered.
 */
MORPHUNTION_CAPI CFStringRef mdr_getDataPathForLocaleCopy(CFLocaleRef locale, CFErrorRef* error);
