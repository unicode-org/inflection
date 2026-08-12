/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <unicode/utypes.h>
/**
 * Registers a path on the filesystem for Inflection to use as the backing data for a given locale.
 *
 * @param locale The locale to register the data for.
 * @param path The path on the filesystem (relative or absolute).
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 */
INFLECTION_CAPI void idr_registerDataPathForLocale(const char* locale, const char* path, UErrorCode* status);
/**
 * Returns the filesystem path that was previously registered for a given locale.
 *
 * @param locale The locale.
 * @param status Must be a valid pointer to an error code value,
 *               which must not indicate a failure before the function call.
 * @return The length of the result string. It may be greater than destCapacity. In that case,
 *         only some of the result was written to the destination buffer.
 */
INFLECTION_CAPI int32_t idr_getDataPathForLocale(const char* locale, char* dest, int32_t destCapacity, UErrorCode* status);
