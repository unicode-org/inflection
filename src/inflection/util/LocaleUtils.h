//
//  LocalUtils.h
//  inflection
//
//  Created by Nolan Kramer on 5/27/21.
//  Copyright 2021-2024 Apple Inc. All rights reserved.
//

#pragma once

#include <inflection/api.h>
#include <unicode/uenum.h>

/**
 * Returns the list of supported locales in Inflection as a <code>UEnumeration</code>.
 * @return The list of supported locales in Inflection.
 */
INFLECTION_CAPI UEnumeration* iloc_getSupportedLocalesList(UErrorCode* status);
