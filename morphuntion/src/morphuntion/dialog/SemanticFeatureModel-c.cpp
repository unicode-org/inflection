/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SemanticFeatureModel.h>
#include <morphuntion//dialog/SemanticFeatureModel.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>

MORPHUNTION_CAPI MDSemanticFeatureModel*
msfmod_create(CFLocaleRef locale, CFErrorRef* error)
{
    try {
        return (MDSemanticFeatureModel*)new ::morphuntion::dialog::SemanticFeatureModel(morphuntion::util::CFUtils::convert(locale));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI void
msfmod_destroy(MDSemanticFeatureModel* thisObject)
{
    delete (::morphuntion::dialog::SemanticFeatureModel*)thisObject;
}
