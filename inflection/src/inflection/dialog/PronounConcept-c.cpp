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
ipron_createWithCustom(const IDSemanticFeatureModel* model, CFArrayRef defaultDisplayData, CFDictionaryRef defaultConstraints, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            ::std::vector<::inflection::dialog::SemanticFeatureModel_DisplayValue> defaultDisplayDataVector;
            ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> defaultConstraintsMap(inflection::dialog::SemanticUtils::to_constraintMap(*npc((const inflection::dialog::SemanticFeatureModel*)model), defaultConstraints));
            CFIndex numMappings = CFArrayGetCount(defaultDisplayData);
            for (CFIndex idx = 0; idx < numMappings; idx++) {
                defaultDisplayDataVector.push_back(*npc((const inflection::dialog::SemanticFeatureModel_DisplayValue*)CFArrayGetValueAtIndex(defaultDisplayData, idx)));
            }
            return (IDPronounConcept*)new inflection::dialog::PronounConcept(*npc((const inflection::dialog::SemanticFeatureModel*)model), defaultDisplayDataVector, defaultConstraintsMap);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDPronounConcept*
ipron_createWithDefaults(const IDSemanticFeatureModel* model, const CFDictionaryRef defaultConstraints, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            ::std::map<inflection::dialog::SemanticFeature, ::std::u16string> defaultConstraintsMap(inflection::dialog::SemanticUtils::to_constraintMap(*npc((const inflection::dialog::SemanticFeatureModel*)model), defaultConstraints));
            return (IDPronounConcept*)new inflection::dialog::PronounConcept(*npc((const inflection::dialog::SemanticFeatureModel*)model), defaultConstraintsMap);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDPronounConcept*
ipron_createFromInitialPronoun(const IDSemanticFeatureModel* model, CFStringRef initialPronoun, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDPronounConcept*)new inflection::dialog::PronounConcept(*npc((const inflection::dialog::SemanticFeatureModel*)model),::inflection::util::TypeConversionUtils::to_u16string(initialPronoun));
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
