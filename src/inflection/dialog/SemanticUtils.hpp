/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>
#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/DisplayValue.h>
#include <string>
#include <map>

class inflection::dialog::SemanticUtils final
{
public:
    static ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> to_constraintMap(const inflection::dialog::SemanticFeatureModel& model, const IDDisplayValue_Constraint* constraints, int32_t constraintsLen);
    SemanticUtils() = delete;
};
