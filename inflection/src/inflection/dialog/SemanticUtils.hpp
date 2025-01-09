/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>
#include <inflection/dialog/fwd.hpp>
#include <CoreFoundation/CFDictionary.h>
#include <string>
#include <map>

class inflection::dialog::SemanticUtils final
{
public:
    static ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> to_constraintMap(const inflection::dialog::SemanticFeatureModel& model, CFDictionaryRef constraints);
    SemanticUtils() = delete;
};
