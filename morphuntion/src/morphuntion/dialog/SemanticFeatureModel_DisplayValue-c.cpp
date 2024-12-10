/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.h>

#include <morphuntion/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticUtils.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/npc.hpp>

MORPHUNTION_CAPI MDSemanticFeatureModel_DisplayValue*
mdv_create(const MDSemanticFeatureModel* model, CFStringRef displayString, CFDictionaryRef constraintMap, CFErrorRef* error)
{
    try {
        ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> constraintsMap(morphuntion::dialog::SemanticUtils::to_constraintMap(*npc((const morphuntion::dialog::SemanticFeatureModel*)model), constraintMap));
        return (MDSemanticFeatureModel_DisplayValue*)new ::morphuntion::dialog::SemanticFeatureModel_DisplayValue(::morphuntion::util::CFUtils::to_u16string(displayString),constraintsMap);
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI void
mdv_destroy(MDSemanticFeatureModel_DisplayValue* thisObject)
{
    delete (::morphuntion::dialog::SemanticFeatureModel_DisplayValue*)thisObject;
}
