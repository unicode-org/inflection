/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/resources/DataRegistrationService.h>

#include <inflection/resources/DataRegistrationService.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

using ::inflection::resources::DataRegistrationService;

INFLECTION_CAPI void
idr_registerDataPathForLocale(const char* locale, CFStringRef path, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale(npc(locale)),
                                                               inflection::util::TypeConversionUtils::to_string(path));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
}

INFLECTION_CAPI CFStringRef
idr_getDataPathForLocaleCopy(const char* locale, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(DataRegistrationService::getDataPathForLocale(inflection::util::ULocale(npc(locale))));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}
