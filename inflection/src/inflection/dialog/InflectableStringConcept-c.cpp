/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/InflectableStringConcept.h>

#include <inflection/dialog/InflectableStringConcept.hpp>
#include <inflection/dialog/SemanticFeatureConcept.h>
#include <inflection/exception/ClassCastException.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/dialog/SemanticFeature.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/Validate.hpp>

INFLECTION_CAPI IDSemanticFeatureConcept* iinf_toSemanticFeatureConcept(IDInflectableStringConcept* thisObject, UErrorCode*)
{
    return (IDSemanticFeatureConcept*)thisObject;
}

INFLECTION_CAPI IDInflectableStringConcept* iinf_toInflectableStringConcept(IDSemanticFeatureConcept* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            auto thisConcept = reinterpret_cast<const ::inflection::Object*>(thisObject);
            if (dynamic_cast<const ::inflection::dialog::InflectableStringConcept*>(thisConcept) != nullptr) {
                return (IDInflectableStringConcept*)thisObject;
            }
            throw inflection::exception::ClassCastException(u"Object is not a PronounConcept");
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDInflectableStringConcept*
iinf_create(const IDSemanticFeatureModel* model, const IDSpeakableString* value, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(value);
            return (IDInflectableStringConcept*)new ::inflection::dialog::InflectableStringConcept((const inflection::dialog::SemanticFeatureModel*)model,*((const inflection::dialog::SpeakableString*)value));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDInflectableStringConcept*
iinf_createWithConstraints(const IDSemanticFeatureModel* model, const IDSpeakableString* value, 
                           const ::inflection::dialog::SemanticFeature* features, const char16_t** values, int32_t SemanticFeaturesLen, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(value);

            ::std::map< ::inflection::dialog::SemanticFeature, ::std::u16string> initialConstraints;
            for (int32_t i = 0; i < SemanticFeaturesLen; ++i) {
                initialConstraints.emplace(features[i], ::std::u16string(values[i]));
            }

            return (IDInflectableStringConcept*) new ::inflection::dialog::InflectableStringConcept(
                (const ::inflection::dialog::SemanticFeatureModel*)model,
                *((const ::inflection::dialog::SpeakableString*)value),
                initialConstraints
            );
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI void
iinf_destroy(IDInflectableStringConcept* thisObject)
{
    delete (::inflection::dialog::InflectableStringConcept*)thisObject;
}
