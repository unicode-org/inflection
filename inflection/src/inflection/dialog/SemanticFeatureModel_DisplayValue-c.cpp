/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticFeatureModel_DisplayValue.h>

#include <inflection/dialog/SemanticFeatureModel_DisplayValue.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI IDSemanticFeatureModel_DisplayValue*
idv_create(const IDSemanticFeatureModel* model, CFStringRef displayString, CFDictionaryRef constraintMap, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> constraintsMap(inflection::dialog::SemanticUtils::to_constraintMap(*npc((const inflection::dialog::SemanticFeatureModel*)model), constraintMap));
            return (IDSemanticFeatureModel_DisplayValue*)new ::inflection::dialog::SemanticFeatureModel_DisplayValue(::inflection::util::TypeConversionUtils::to_u16string(displayString), constraintsMap);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI void
idv_destroy(IDSemanticFeatureModel_DisplayValue* thisObject)
{
    delete (::inflection::dialog::SemanticFeatureModel_DisplayValue*)thisObject;
}
