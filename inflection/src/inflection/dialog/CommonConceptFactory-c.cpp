/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/CommonConceptFactory.h>

#include <inflection/dialog/CommonConceptFactory.hpp>
#include <inflection/dialog/SpeakableConcept.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>


INFLECTION_CAPI const IDSemanticFeatureModel* iccf_getSemanticFeatureModel(const IDCommonConceptFactory *thisObject, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (const IDSemanticFeatureModel*)(npc((const ::inflection::dialog::CommonConceptFactory*)thisObject)->getSemanticFeatureModel());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI const char* iccf_getLanguage(const IDCommonConceptFactory* thisObject, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return npc((const ::inflection::dialog::CommonConceptFactory *) thisObject)->getLanguage().getName().c_str();
        }
        catch (const ::std::exception &e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString* iccf_quote(const IDCommonConceptFactory* thisObject, const IDSpeakableString* str, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*) new ::inflection::dialog::SpeakableString(
                    npc((const ::inflection::dialog::CommonConceptFactory*)thisObject)->quote(*npc((const inflection::dialog::SpeakableString*) str))
                );
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString* iccf_quantifyCopy(const IDCommonConceptFactory* thisObject, const IDNumberConcept* number, const IDSemanticFeatureConcept* semanticConcept, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*)(npc((const ::inflection::dialog::CommonConceptFactory*)thisObject)->quantify( *npc((const inflection::dialog::NumberConcept*) number),(const inflection::dialog::SemanticFeatureConceptBase*) semanticConcept));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString* iccf_quantifyFormattedCopy(const IDCommonConceptFactory* thisObject, const IDNumberConcept* number, const IDSpeakableString* formattedNumber, const IDSemanticFeatureConcept* semanticConcept, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*)(npc((const ::inflection::dialog::CommonConceptFactory*)thisObject)->quantifyFormatted(*npc((const inflection::dialog::NumberConcept*) number),*npc((const inflection::dialog::SpeakableString*) formattedNumber),(const inflection::dialog::SemanticFeatureConceptBase*) semanticConcept));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}
