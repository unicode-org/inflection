/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <morphuntion/dialog/SemanticFeatureModel.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFString.h>

MORPHUNTION_CTYPE(MDSemanticFeatureModel_DisplayValue)

/**
 * Construct a display value with a string and the associated constraints.
 * @param displayString A value with no speak information. The value is copied.
 * @param constraintMap All constraints that match the displayString. The values are copied.
 * @param error The provided error reference is allocated when the function failed.
 */
MORPHUNTION_CAPI MDSemanticFeatureModel_DisplayValue* mdv_create(const MDSemanticFeatureModel* model, CFStringRef displayString, CFDictionaryRef constraintMap, CFErrorRef* error);
/**
 * The destructor
 */
MORPHUNTION_CAPI void mdv_destroy(MDSemanticFeatureModel_DisplayValue* thisObject);
