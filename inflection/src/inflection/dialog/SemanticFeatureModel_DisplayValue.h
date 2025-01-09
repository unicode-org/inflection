/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/SemanticFeatureModel.h>
#include <CoreFoundation/CFString.h>

INFLECTION_CTYPE(IDSemanticFeatureModel_DisplayValue)

/**
 * Construct a display value with a string and the associated constraints.
 * @param displayString A value with no speak information. The value is copied.
 * @param constraintMap All constraints that match the displayString. The values are copied.
 * @param error The provided error reference is allocated when the function failed.
 */
INFLECTION_CAPI IDSemanticFeatureModel_DisplayValue* idv_create(const IDSemanticFeatureModel* model, CFStringRef displayString, CFDictionaryRef constraintMap, UErrorCode* status);
/**
 * The destructor
 */
INFLECTION_CAPI void idv_destroy(IDSemanticFeatureModel_DisplayValue* thisObject);
