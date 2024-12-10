/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFLocale.h>
#include <CoreFoundation/CFSet.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFString.h>

/**
 * A semantic model that contains display data for semantic values and provides
 * feature functions for deriving missing data.
 */
MORPHUNTION_CTYPE(MDSemanticFeatureModel);
/**
 * Creates a default SemanticFeatureModel for a given locale without any display data.
 */
MORPHUNTION_CAPI MDSemanticFeatureModel* msfmod_create(CFLocaleRef locale, CFErrorRef* error);
/**
 * Destructor.
 */
MORPHUNTION_CAPI void msfmod_destroy(MDSemanticFeatureModel* thisObject);
