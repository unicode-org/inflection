/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <inflection/dialog/CommonConceptFactory.h>

/**
 * Provides a way to retrieve a CommonConceptFactory
 */
INFLECTION_CTYPE(IDLocalizedCommonConceptFactoryProvider);
/**
 * Returns a CommonConceptFactory for the requested locale.
 */
INFLECTION_CAPI const IDCommonConceptFactory* ilccfp_getCommonConceptFactory(IDLocalizedCommonConceptFactoryProvider* thisObject, const char* locale, UErrorCode* status);
/**
 * Returns a LocalizedCommonConceptFactoryProvider singleton.
 */
INFLECTION_CAPI IDLocalizedCommonConceptFactoryProvider* ilccfp_getDefaultCommonConceptFactoryProvider(UErrorCode* status);
