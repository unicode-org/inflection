/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <inflection/resources/DataRegistrationService.h>

#include <inflection/resources/DataRegistrationService.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>
#include <string.h>

using ::inflection::resources::DataRegistrationService;

INFLECTION_CAPI void
idr_registerDataPathForLocale(const char* locale, const char* path, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            DataRegistrationService::registerDataPathForLocale(inflection::util::ULocale(npc(locale)), path);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
}

INFLECTION_CAPI int32_t
idr_getDataPathForLocale(const char* locale, char* dest, int32_t destCapacity, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(dest);
            auto result(DataRegistrationService::getDataPathForLocale(inflection::util::ULocale(npc(locale))));
            if (destCapacity > 0 && int32_t(result.length()) < destCapacity) {
                strncpy(dest, result.c_str(), result.length());
            }
            return inflection::util::TypeConversionUtils::terminateCharString(dest, destCapacity, int32_t(result.length()), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}
