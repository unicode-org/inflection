/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/resources/DataRegistrationService.h>

#include <morphuntion/resources/DataRegistrationService.hpp>
#include <morphuntion/util/CFUtils.hpp>

using ::morphuntion::resources::DataRegistrationService;

MORPHUNTION_CAPI void
mdr_registerDataPathForLocale(CFLocaleRef locale, CFStringRef path, CFErrorRef* error)
{
    try {
        DataRegistrationService::registerDataPathForLocale(morphuntion::util::CFUtils::convert(locale),
                                                           morphuntion::util::CFUtils::to_string(path));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
}

MORPHUNTION_CAPI CFStringRef
mdr_getDataPathForLocaleCopy(CFLocaleRef locale, CFErrorRef* error)
{
    try {
        return morphuntion::util::CFUtils::to_CFString(DataRegistrationService::getDataPathForLocale(morphuntion::util::CFUtils::convert(locale)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}
