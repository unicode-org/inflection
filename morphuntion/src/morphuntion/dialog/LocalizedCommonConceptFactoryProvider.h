/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#pragma once
#include <morphuntion/api.h>
#include <morphuntion/dialog/CommonConceptFactory.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFLocale.h>

/**
 * Provides a way to retrieve a CommonConceptFactory
 */
MORPHUNTION_CTYPE(MDLocalizedCommonConceptFactoryProvider);
/**
 * Returns a CommonConceptFactory for the requested locale.
 */
MORPHUNTION_CAPI const MDCommonConceptFactory* mlccfp_getCommonConceptFactory(MDLocalizedCommonConceptFactoryProvider* thisObject, CFLocaleRef locale, CFErrorRef* error);
/**
 * Returns a LocalizedCommonConceptFactoryProvider singleton.
 */
MORPHUNTION_CAPI MDLocalizedCommonConceptFactoryProvider* mlccfp_getDefaultCommonConceptFactoryProvider(CFErrorRef* error);
