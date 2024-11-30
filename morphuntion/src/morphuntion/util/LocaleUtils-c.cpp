/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/LocaleUtils.h>
#include <morphuntion/util/LocaleUtils.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/util/ULocale.hpp>

#include <set>

extern "C" {
static void mloc_array_release(CFAllocatorRef, const void *value)
{
    CFRelease(value);
}
}

using morphuntion::util::LocaleUtils;
using morphuntion::util::ULocale;
using morphuntion::util::CFUtils;

MORPHUNTION_CAPI CFArrayRef mloc_getSupportedLocalesList()
{
    static const CFArrayCallBacks callbacks
    {
        0,
        nullptr,
        mloc_array_release,
        nullptr,
        nullptr
    };

    std::set<ULocale> morphunLocales = LocaleUtils::getSupportedLocaleList();
    std::vector<CFLocaleRef> cfLocales;
    cfLocales.reserve(morphunLocales.size());

    for(const auto& iter : morphunLocales)
    {
        cfLocales.emplace_back(CFUtils::convert(iter));
        // Remove extra ref
        CFRelease(cfLocales.back());
    }

    return CFArrayCreate(nullptr, (const void**)cfLocales.data(), (CFIndex)cfLocales.size(), &callbacks);
}

CFLocaleRef mloc_createValidRegionAwareLanguage(CFLocaleRef language, CFLocaleRef region) {
    return CFUtils::convert(LocaleUtils::createValidRegionAwareLanguage(CFUtils::convert(language), CFUtils::convert(region)));
}
