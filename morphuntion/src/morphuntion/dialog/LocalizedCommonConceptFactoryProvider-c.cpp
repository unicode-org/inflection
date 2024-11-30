/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/LocalizedCommonConceptFactoryProvider.h>
#include <morphuntion/npc.hpp>
#include <morphuntion//dialog/LocalizedCommonConceptFactoryProvider.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>

MORPHUNTION_CAPI const MDCommonConceptFactory*
mlccfp_getCommonConceptFactory(MDLocalizedCommonConceptFactoryProvider* thisObject, CFLocaleRef locale, CFErrorRef* error)
{
    try {
        return (const MDCommonConceptFactory*)(npc((::morphuntion::dialog::LocalizedCommonConceptFactoryProvider*)thisObject)->getCommonConceptFactory(morphuntion::util::CFUtils::convert(locale)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDLocalizedCommonConceptFactoryProvider*
mlccfp_getDefaultCommonConceptFactoryProvider(CFErrorRef* error)
{
    try {
        return (MDLocalizedCommonConceptFactoryProvider*)(npc(::morphuntion::dialog::LocalizedCommonConceptFactoryProvider::getDefaultCommonConceptFactoryProvider()));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

