/*
 * Copyright 2024 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/dialog/DisplayValue.h>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI bool
idv_isValidConstraints(const IDSemanticFeatureModel* model,
                       const IDDisplayValue_Constraint* constraints,
                       int32_t constraintsLen,
                       UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            if (constraints == nullptr) {
                return constraintsLen <= 0;
            }
            const auto& modelRef = *npc((const inflection::dialog::SemanticFeatureModel*)model);
            for (int32_t idx = 0; idx < constraintsLen || (constraintsLen < 0 && constraints[idx].name != nullptr); idx++) {
                const auto& constraint = constraints[idx];
                if (constraint.name == nullptr || constraint.value == nullptr) {
                    return false;
                }
                std::u16string_view keyStr(constraint.name);
                auto feature = modelRef.getFeature(keyStr);
                if (feature == nullptr) {
                    return false;
                }
                const auto& values(feature->getBoundedValues());
                if (!values.empty() && !values.contains(constraint.value)) {
                    return false;
                }
            }
            return true;
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI bool
idv_isValidDisplayValue(const IDSemanticFeatureModel* model,
                        const IDDisplayValue* displayValue,
                        UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        if (model != nullptr && displayValue != nullptr && displayValue->displayString != nullptr) {
            return idv_isValidConstraints(model, displayValue->constraintMap, displayValue->constraintMapLen, status);
        }
    }
    return false;
}
