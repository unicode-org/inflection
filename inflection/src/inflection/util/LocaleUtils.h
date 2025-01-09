//
//  LocalUtils.h
//  inflection
//
//  Created by Nolan Kramer on 5/27/21.
//  Copyright 2021-2024 Apple Inc. All rights reserved.
//

#pragma once

#include <inflection/api.h>
#include <CoreFoundation/CFArray.h>
#include <unicode/utypes.h>

/**
 * Returns the list of supported locales in Inflection as a <code>std::set<ULocale></code>.
 * @return The list of supported locales in Inflection.
 */
INFLECTION_CAPI CFArrayRef iloc_getSupportedLocalesList(UErrorCode* status);
