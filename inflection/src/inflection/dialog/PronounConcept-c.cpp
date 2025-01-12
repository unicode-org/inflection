/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/PronounConcept.h>

#include <inflection/dialog/PronounConcept.hpp>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/dialog/SemanticUtils.hpp>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI IDSemanticFeatureConcept* ipron_toSemanticFeatureConcept(IDPronounConcept* thisObject, UErrorCode*)
{
    return (IDSemanticFeatureConcept*)thisObject;
}

INFLECTION_CAPI IDPronounConcept* ipron_toPronounConcept(IDSemanticFeatureConcept* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            auto thisConcept = reinterpret_cast<::inflection::dialog::SpeakableConcept*>(thisObject);
            auto pronounConcept = dynamic_cast<::inflection::dialog::PronounConcept*>(thisConcept);
            if (pronounConcept != nullptr) {
                return (IDPronounConcept*)pronounConcept;
            }
            throw inflection::exception::ClassCastException(u"Object is not a PronounConcept");
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDPronounConcept*
ipron_createWithCustom(const IDSemanticFeatureModel* model,
                       const IDDisplayValue* defaultDisplayData,
                       int32_t defaultDisplayDataLen,
                       const IDDisplayValue_Constraint* defaultConstraints,
                       int32_t defaultConstraintsLen,
                       UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            const auto& modelRef = *npc((const inflection::dialog::SemanticFeatureModel*)model);
            ::std::vector<::inflection::dialog::DisplayValue> defaultDisplayDataVector;
            if (defaultDisplayDataLen > 0) {
                defaultDisplayDataVector.reserve(defaultDisplayDataLen);
                for (int32_t idx = 0; idx < defaultDisplayDataLen; idx++) {
                    const auto& displayValue = defaultDisplayData[idx];
                    defaultDisplayDataVector.emplace_back(displayValue.displayStringLen < 0
                                                       ? displayValue.displayString
                                                       : std::u16string(displayValue.displayString, displayValue.displayStringLen),
                                                       inflection::dialog::SemanticUtils::to_constraintMap(modelRef, displayValue.constraintMap, displayValue.constraintMapLen));
                }
            }
            auto defaultConstraintsMap(inflection::dialog::SemanticUtils::to_constraintMap(modelRef, defaultConstraints, defaultConstraintsLen));
            return (IDPronounConcept*)new inflection::dialog::PronounConcept(modelRef, defaultDisplayDataVector, defaultConstraintsMap);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDPronounConcept*
ipron_createWithDefaults(const IDSemanticFeatureModel* model,
                         const IDDisplayValue_Constraint* defaultConstraints,
                         int32_t defaultConstraintsLen,
                         UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            auto defaultConstraintsMap(inflection::dialog::SemanticUtils::to_constraintMap(*npc((const inflection::dialog::SemanticFeatureModel*)model), defaultConstraints, defaultConstraintsLen));
            return (IDPronounConcept*)new inflection::dialog::PronounConcept(*npc((const inflection::dialog::SemanticFeatureModel*)model), defaultConstraintsMap);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDPronounConcept*
ipron_createFromInitialPronoun(const IDSemanticFeatureModel* model, const char16_t* initialPronoun, int32_t initialPronounLen, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDPronounConcept*)new inflection::dialog::PronounConcept(*npc((const inflection::dialog::SemanticFeatureModel*)model),
                                                                             initialPronounLen < 0
                                                                             ? ::std::u16string(initialPronoun)
                                                                             : ::std::u16string(initialPronoun, initialPronounLen));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI void
ipron_destroy(IDPronounConcept* thisObject)
{
    delete (::inflection::dialog::PronounConcept*)thisObject;
}

INFLECTION_CAPI bool
ipron_isCustomMatch(const IDPronounConcept* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return npc((const inflection::dialog::PronounConcept*)thisObject)->isCustomMatch();
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}
