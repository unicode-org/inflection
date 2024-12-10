//
//  LocalUtils.h
//  morphuntion
//
//  Created by Nolan Kramer on 5/27/21.
//  Copyright 2021-2024 Apple Inc. All rights reserved.
//

#pragma once

#include <morphuntion/api.h>
#include <CoreFoundation/CFError.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFLocale.h>

/**
 * Returns the list of supported locales in Morphuntion as a <code>std::set<ULocale></code>.
 * @return The list of supported locales in Morphuntion.
 */
MORPHUNTION_CAPI CFArrayRef mloc_getSupportedLocalesList();

/**
 * This allows you to have behavior that is close to the spoken language, without forcing an unsupported language variant from being too strictly the region based language.
 * If your iOS language is en_US, but your spoken language is ar_SA, just return ar_SA because ar_US is not valid in CLDR.
 * If your iOS language is ar_TN, but your spoken language is ar_SA, return ar_TN because it's valid in CLDR and ICU.
 * For example, ar_TN uses 0-9 for the digits, but ar_SA uses ٠-٩ for the digits.
 * If your iOS language is es_MX, but your spoken language is es_CO, return es_MX because it's valid in CLDR and ICU.
 * For example, es_MX uses "." for the decimal point, but es_CO uses "," for the decimal point.
 * @param language The spoken language setting
 * @param region The region setting from the device, which includes the iOS language
 * @return A hybrid locale
 */
MORPHUNTION_CAPI CFLocaleRef mloc_createValidRegionAwareLanguage(CFLocaleRef language, CFLocaleRef region);
