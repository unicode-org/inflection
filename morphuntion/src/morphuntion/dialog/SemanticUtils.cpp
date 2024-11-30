/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SemanticUtils.hpp>

#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>

namespace morphuntion::dialog {

typedef struct ConstraintMap {
    const morphuntion::dialog::SemanticFeatureModel& model;
    ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string>& constraintsMap;
    ::std::u16string error {  };
    ::std::u16string* errorPtr {  };
} ConstraintMap;

} // namespace morphuntion::dialog

/**
 * This is used when iterating over a CFDictionary to convert each entry to a C++ map.
 */
extern "C" {
static void convertSemanticFeatureAndString(const void* key, const void* value, void* constraintsMap) {
    try {
        auto keyStr(::morphuntion::util::CFUtils::to_u16string((CFStringRef)key));
        auto featureName = npc((morphuntion::dialog::ConstraintMap*)constraintsMap)->model.getFeature(keyStr);
        morphuntion::util::Validate::notNull(featureName, keyStr);
        npc((morphuntion::dialog::ConstraintMap*)constraintsMap)->constraintsMap.emplace(*npc(featureName), ::morphuntion::util::CFUtils::to_u16string((CFStringRef)value));
    }
    catch (const morphuntion::exception::Throwable& e) {
        ((morphuntion::dialog::ConstraintMap*)constraintsMap)->error = e.getMessage();
        ((morphuntion::dialog::ConstraintMap*)constraintsMap)->errorPtr = &((morphuntion::dialog::ConstraintMap*)constraintsMap)->error;
    }
}
}

namespace morphuntion::dialog {

::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> SemanticUtils::to_constraintMap(const morphuntion::dialog::SemanticFeatureModel& model, CFDictionaryRef constraints)
{
    ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> result;
    if (constraints != nullptr) {
        ConstraintMap constraintMapStruct = {
                model,
                result
        };
        CFDictionaryApplyFunction(constraints, convertSemanticFeatureAndString, &constraintMapStruct);
        if (constraintMapStruct.errorPtr != nullptr) {
            throw morphuntion::exception::RuntimeException(constraintMapStruct.error);
        }
    }
    return result;
}

} // namespace morphuntion::dialog
