/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <inflection/npc.hpp>
#include <inflection/dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>

INFLECTION_CAPI const IDCommonConceptFactory*
ilccfp_getCommonConceptFactory(IDLocalizedCommonConceptFactoryProvider* thisObject, const char* locale, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (const IDCommonConceptFactory*)(npc((::inflection::dialog::LocalizedCommonConceptFactoryProvider*)thisObject)->getCommonConceptFactory(inflection::util::ULocale(npc(locale))));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDLocalizedCommonConceptFactoryProvider*
ilccfp_getDefaultCommonConceptFactoryProvider(UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDLocalizedCommonConceptFactoryProvider*)(npc(::inflection::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

