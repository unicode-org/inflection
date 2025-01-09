/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/LocaleUtils.h>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>

#include <set>

extern "C" {
static void mloc_array_release(CFAllocatorRef, const void *value)
{
    free((void *)value);
}
}

using inflection::util::LocaleUtils;
using inflection::util::ULocale;
using inflection::util::TypeConversionUtils;

INFLECTION_CAPI CFArrayRef iloc_getSupportedLocalesList(UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            static const CFArrayCallBacks callbacks
            {
                0,
                nullptr,
                mloc_array_release,
                nullptr,
                nullptr
            };

            std::set<ULocale> morphunLocales = LocaleUtils::getSupportedLocaleList();
            std::vector<const char*> cfLocales;
            cfLocales.reserve(morphunLocales.size());

            for (const auto& iter : morphunLocales)
            {
                cfLocales.emplace_back(npc(strdup(iter.getName().c_str())));
            }

            return CFArrayCreate(nullptr, (const void**)cfLocales.data(), (CFIndex)cfLocales.size(), &callbacks);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}
