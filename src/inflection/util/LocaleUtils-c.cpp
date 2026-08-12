/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/LocaleUtils.h>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <set>
#include <string.h>

using inflection::util::LocaleUtils;
using inflection::util::ULocale;

static std::pair<char*const*, int32_t> createLocaleList() {
    auto inflectionLocales(LocaleUtils::getSupportedLocaleList());
    char** array = new char*[inflectionLocales.size()];
    int32_t idx = 0;
    int32_t fullSize = 0;
    // Calculate the size.
    for (const auto& iter : inflectionLocales)
    {
        fullSize += int32_t(iter.getName().length() + 1); // +1 for null
    }
    // allocate 1 block for all strings. This can reduce the overall size and reduce heap fragmentation.
    char* buffPtr = new char[fullSize];
    // Populate the array with strings for the locales.
    for (const auto& iter : inflectionLocales)
    {
        const auto& locale(iter.getName());
        array[idx++] = buffPtr;
        strcpy(buffPtr, locale.c_str());
        buffPtr += locale.length() + 1; // +1 for null
    }
    return {array, inflectionLocales.size()};
}

static std::pair<char*const*, int32_t> LOCALE_LIST() {
    static auto LOCALE_LIST_ = new std::pair<char*const*, int32_t>(createLocaleList());
    return *npc(LOCALE_LIST_);
}

INFLECTION_CAPI UEnumeration* iloc_getSupportedLocalesList(UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            auto localeList(LOCALE_LIST());

            return uenum_openCharStringsEnumeration(localeList.first, localeList.second, status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}
