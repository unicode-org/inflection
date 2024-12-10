/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/InflectableStringConcept.h>

#include <morphuntion/dialog/InflectableStringConcept.hpp>
#include <morphuntion/dialog/SemanticFeatureConcept.h>
#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/util/Validate.hpp>
#include <morphuntion/npc.hpp>

MORPHUNTION_CAPI MDSemanticFeatureConcept* minf_toSemanticFeatureConcept(MDInflectableStringConcept* thisObject, CFErrorRef*)
{
    return (MDSemanticFeatureConcept*)thisObject;
}

MORPHUNTION_CAPI MDInflectableStringConcept* minf_toInflectableStringConcept(MDSemanticFeatureConcept* thisObject, CFErrorRef* error)
{
    try {
        auto thisConcept = reinterpret_cast<const ::morphuntion::Object*>(thisObject);
        if (dynamic_cast<const ::morphuntion::dialog::InflectableStringConcept*>(thisConcept) != nullptr) {
            return (MDInflectableStringConcept*)thisObject;
        }
        throw morphuntion::exception::ClassCastException(u"Object is not a PronounConcept");
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString*
minf_toSpeakableString(MDInflectableStringConcept* thisObject, CFErrorRef* error)
{
    try {
        return (MDSpeakableString*)(npc((::morphuntion::dialog::InflectableStringConcept*)thisObject)->toSpeakableString());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDInflectableStringConcept*
minf_clone(MDInflectableStringConcept* thisObject, CFErrorRef* error)
{
    try {
        return (MDInflectableStringConcept*)(npc((::morphuntion::dialog::InflectableStringConcept*)thisObject)->clone());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDInflectableStringConcept*
minf_create(const MDSemanticFeatureModel* model, const MDSpeakableString* value, CFErrorRef* error)
{
    try {
        morphuntion::util::Validate::notNull(value);
        return (MDInflectableStringConcept*)new ::morphuntion::dialog::InflectableStringConcept((const morphuntion::dialog::SemanticFeatureModel*)model,*((const morphuntion::dialog::SpeakableString*)value));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI void
minf_destroy(MDInflectableStringConcept* thisObject)
{
    delete (::morphuntion::dialog::InflectableStringConcept*)thisObject;
}

MORPHUNTION_CAPI bool
minf_isExists(MDInflectableStringConcept* thisObject, CFErrorRef* error)
{
     try {
         return npc((::morphuntion::dialog::InflectableStringConcept*)thisObject)->isExists();
     }
     catch (const ::std::exception& e) {
         morphuntion::util::CFUtils::convert(e, error);
     }
     return false;
}

MORPHUNTION_CAPI void
minf_putConstraintByName(MDInflectableStringConcept* thisObject, CFStringRef featureName, CFStringRef featureValue, CFErrorRef* error)
{
    try {
        (npc((::morphuntion::dialog::InflectableStringConcept*)thisObject)->putConstraintByName(::morphuntion::util::CFUtils::to_u16string(featureName),::morphuntion::util::CFUtils::to_u16string(featureValue)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
}

MORPHUNTION_CAPI void
minf_clearConstraintByName(MDInflectableStringConcept* thisObject, CFStringRef featureName, CFErrorRef* error)
{
    try {
        (npc((::morphuntion::dialog::InflectableStringConcept*)thisObject)->clearConstraintByName(::morphuntion::util::CFUtils::to_u16string(featureName)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
}

MORPHUNTION_CAPI void
minf_reset(MDInflectableStringConcept* thisObject, CFErrorRef* error)
{
    try {
        (npc((::morphuntion::dialog::InflectableStringConcept*)thisObject)->reset());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
}

MORPHUNTION_CAPI MDSpeakableString*
minf_getFeatureValueByNameCopy(MDInflectableStringConcept* thisObject, CFStringRef featureName, CFErrorRef* error)
{
    try {
        return (MDSpeakableString*)(npc((::morphuntion::dialog::InflectableStringConcept*)thisObject)->getFeatureValueByName(::morphuntion::util::CFUtils::to_u16string(featureName)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

