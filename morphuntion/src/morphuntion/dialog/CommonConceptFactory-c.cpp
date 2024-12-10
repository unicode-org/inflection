/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/CommonConceptFactory.h>

#include <morphuntion/dialog/CommonConceptFactory.hpp>
#include <morphuntion/dialog/SpeakableConcept.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/npc.hpp>


MORPHUNTION_CAPI const MDSemanticFeatureModel* mccf_getSemanticFeatureModel(const MDCommonConceptFactory *thisObject, CFErrorRef* error) {
    try {
        return (const MDSemanticFeatureModel*)(npc((const ::morphuntion::dialog::CommonConceptFactory*)thisObject)->getSemanticFeatureModel());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFLocaleRef mccf_getLanguageCopy(const MDCommonConceptFactory* thisObject, CFErrorRef* error) {
    try {
        return morphuntion::util::CFUtils::convert(npc((const ::morphuntion::dialog::CommonConceptFactory*)thisObject)->getLanguage());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString* mccf_quote(const MDCommonConceptFactory* thisObject, const MDSpeakableString* str, CFErrorRef* error) {
    try {
        return (MDSpeakableString*) new ::morphuntion::dialog::SpeakableString(
                npc((const ::morphuntion::dialog::CommonConceptFactory*)thisObject)->quote(*npc((const morphuntion::dialog::SpeakableString*) str))
            );
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString* mccf_quantifyCopy(const MDCommonConceptFactory* thisObject, const MDNumberConcept* number, const MDSemanticFeatureConcept* semanticConcept, CFErrorRef* error) {
    try {
        return (MDSpeakableString*)(npc((const ::morphuntion::dialog::CommonConceptFactory*)thisObject)->quantify( *npc((const morphuntion::dialog::NumberConcept*) number),(const morphuntion::dialog::SemanticFeatureConceptBase*) semanticConcept));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString* mccf_quantifyFormattedCopy(const MDCommonConceptFactory* thisObject, const MDNumberConcept* number, const MDSpeakableString* formattedNumber, const MDSemanticFeatureConcept* semanticConcept, CFErrorRef* error) {
    try {
        return (MDSpeakableString*)(npc((const ::morphuntion::dialog::CommonConceptFactory*)thisObject)->quantify(*npc((const morphuntion::dialog::NumberConcept*) number),*npc((const morphuntion::dialog::SpeakableString*) formattedNumber),(const morphuntion::dialog::SemanticFeatureConceptBase*) semanticConcept));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}
