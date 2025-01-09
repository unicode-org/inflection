/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticUtils.hpp>

#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog {

typedef struct ConstraintMap {
    const inflection::dialog::SemanticFeatureModel& model;
    ::std::map<inflection::dialog::SemanticFeature, ::std::u16string>& constraintsMap;
    ::std::u16string error {  };
    ::std::u16string* errorPtr {  };
} ConstraintMap;

} // namespace inflection::dialog

/**
 * This is used when iterating over a CFDictionary to convert each entry to a C++ map.
 */
extern "C" {
static void convertSemanticFeatureAndString(const void* key, const void* value, void* constraintsMap) {
    try {
        auto keyStr(::inflection::util::TypeConversionUtils::to_u16string((CFStringRef)key));
        auto featureName = npc((inflection::dialog::ConstraintMap*)constraintsMap)->model.getFeature(keyStr);
        inflection::util::Validate::notNull(featureName, keyStr);
        npc((inflection::dialog::ConstraintMap*)constraintsMap)->constraintsMap.emplace(*npc(featureName), ::inflection::util::TypeConversionUtils::to_u16string((CFStringRef)value));
    }
    catch (const inflection::exception::Throwable& e) {
        ((inflection::dialog::ConstraintMap*)constraintsMap)->error = e.getMessage();
        ((inflection::dialog::ConstraintMap*)constraintsMap)->errorPtr = &((inflection::dialog::ConstraintMap*)constraintsMap)->error;
    }
}
}

namespace inflection::dialog {

::std::map<inflection::dialog::SemanticFeature, ::std::u16string> SemanticUtils::to_constraintMap(const inflection::dialog::SemanticFeatureModel& model, CFDictionaryRef constraints)
{
    ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> result;
    if (constraints != nullptr) {
        ConstraintMap constraintMapStruct = {
                model,
                result
        };
        CFDictionaryApplyFunction(constraints, convertSemanticFeatureAndString, &constraintMapStruct);
        if (constraintMapStruct.errorPtr != nullptr) {
            throw inflection::exception::RuntimeException(constraintMapStruct.error);
        }
    }
    return result;
}

} // namespace inflection::dialog
