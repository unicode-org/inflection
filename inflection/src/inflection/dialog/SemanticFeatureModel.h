/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <unicode/utypes.h>

/**
 * A semantic model that contains display data for semantic values and provides
 * feature functions for deriving missing data.
 */
INFLECTION_CTYPE(IDSemanticFeatureModel);
/**
 * Creates a default SemanticFeatureModel for a given locale without any display data.
 */
INFLECTION_CAPI IDSemanticFeatureModel* isfmod_create(const char* locale, UErrorCode* status);
/**
 * Destructor.
 */
INFLECTION_CAPI void isfmod_destroy(IDSemanticFeatureModel* thisObject);
