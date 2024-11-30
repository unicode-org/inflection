/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/api.h>
#include <morphuntion/dialog/fwd.hpp>
#include <CoreFoundation/CFDictionary.h>
#include <string>
#include <map>

class morphuntion::dialog::SemanticUtils final
{
public:
    static ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> to_constraintMap(const morphuntion::dialog::SemanticFeatureModel& model, CFDictionaryRef constraints);
    SemanticUtils() = delete;
};
