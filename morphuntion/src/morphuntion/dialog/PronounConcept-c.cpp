/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/PronounConcept.h>

#include <morphuntion/dialog/PronounConcept.hpp>
#include <morphuntion/dialog/SemanticFeatureModel.hpp>
#include <morphuntion/dialog/SemanticUtils.hpp>
#include <morphuntion/exception/ClassCastException.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/npc.hpp>

MORPHUNTION_CAPI MDSemanticFeatureConcept* mpron_toSemanticFeatureConcept(MDPronounConcept* thisObject, CFErrorRef*)
{
    return (MDSemanticFeatureConcept*)thisObject;
}

MORPHUNTION_CAPI MDPronounConcept* mpron_toPronounConcept(MDSemanticFeatureConcept* thisObject, CFErrorRef* error)
{
    try {
        auto thisConcept = reinterpret_cast<::morphuntion::dialog::SpeakableConcept*>(thisObject);
        auto pronounConcept = dynamic_cast<::morphuntion::dialog::PronounConcept*>(thisConcept);
        if (pronounConcept != nullptr) {
            return (MDPronounConcept*)pronounConcept;
        }
        throw morphuntion::exception::ClassCastException(u"Object is not a PronounConcept");
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDPronounConcept*
mpron_createWithCustom(const MDSemanticFeatureModel* model, CFArrayRef defaultDisplayData, CFDictionaryRef defaultConstraints, CFErrorRef* error)
{
    try {
        ::std::vector<::morphuntion::dialog::SemanticFeatureModel_DisplayValue> defaultDisplayDataVector;
        ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> defaultConstraintsMap(morphuntion::dialog::SemanticUtils::to_constraintMap(*npc((const morphuntion::dialog::SemanticFeatureModel*)model), defaultConstraints));
        CFIndex numMappings = CFArrayGetCount(defaultDisplayData);
        for (CFIndex idx = 0; idx < numMappings; idx++) {
            defaultDisplayDataVector.push_back(*npc((const morphuntion::dialog::SemanticFeatureModel_DisplayValue*)CFArrayGetValueAtIndex(defaultDisplayData, idx)));
        }
        return (MDPronounConcept*)new morphuntion::dialog::PronounConcept(*npc((const morphuntion::dialog::SemanticFeatureModel*)model), defaultDisplayDataVector, defaultConstraintsMap);
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDPronounConcept*
mpron_createWithDefaults(const MDSemanticFeatureModel* model, const CFDictionaryRef defaultConstraints, CFErrorRef* error)
{
    try {
        ::std::map<morphuntion::dialog::SemanticFeature, ::std::u16string> defaultConstraintsMap(morphuntion::dialog::SemanticUtils::to_constraintMap(*npc((const morphuntion::dialog::SemanticFeatureModel*)model), defaultConstraints));
        return (MDPronounConcept*)new morphuntion::dialog::PronounConcept(*npc((const morphuntion::dialog::SemanticFeatureModel*)model), defaultConstraintsMap);
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDPronounConcept*
mpron_createFromInitialPronoun(const MDSemanticFeatureModel* model, CFStringRef initialPronoun, CFErrorRef* error)
{
    try {
        return (MDPronounConcept*)new morphuntion::dialog::PronounConcept(*npc((const morphuntion::dialog::SemanticFeatureModel*)model),::morphuntion::util::CFUtils::to_u16string(initialPronoun));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI void
mpron_destroy(MDPronounConcept* thisObject)
{
    delete (::morphuntion::dialog::PronounConcept*)thisObject;
}

MORPHUNTION_CAPI bool
mpron_isCustomMatch(const MDPronounConcept* thisObject, CFErrorRef* error)
{
    try {
        return npc((const morphuntion::dialog::PronounConcept*)thisObject)->isCustomMatch();
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return false;
}
