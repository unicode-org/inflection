/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticUtils.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog {

::std::map<inflection::dialog::SemanticFeature, ::std::u16string>
SemanticUtils::to_constraintMap(const inflection::dialog::SemanticFeatureModel& model, const IDDisplayValue_Constraint* constraints, int32_t constraintsLen)
{
    ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> result;
    if (constraints != nullptr) {
        for (int32_t idx = 0; idx < constraintsLen || (constraintsLen < 0 && constraints[idx].name != nullptr); idx++) {
            inflection::util::Validate::notNull(constraints[idx].name);
            std::u16string_view keyStr(constraints[idx].name);
            auto featureName = model.getFeature(keyStr);
            inflection::util::Validate::notNull(featureName, keyStr);
            inflection::util::Validate::notNull(constraints[idx].value, keyStr);
            result.emplace(*npc(featureName), constraints[idx].value);
        }
    }
    return result;
}

} // namespace inflection::dialog
