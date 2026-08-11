/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/NumberConcept.h>

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI int32_t inum_toString(const IDNumberConcept* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::copyString(dest, destCapacity, npc((const ::inflection::dialog::NumberConcept*)thisObject)->toString(), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}

INFLECTION_CAPI int32_t inum_asWords(const IDNumberConcept* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::copyString(dest, destCapacity, npc((const ::inflection::dialog::NumberConcept*)thisObject)->getAsWords(), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}

INFLECTION_CAPI int32_t inum_asWordsVariant(const IDNumberConcept* thisObject, const char16_t* variant, char16_t* dest, int32_t destCapacity, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::copyString(dest, destCapacity, npc((const ::inflection::dialog::NumberConcept*)thisObject)->asWords(npc(variant)), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}

INFLECTION_CAPI IDSpeakableString* inum_asSpokenWordsCopy(const IDNumberConcept* thisObject, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*) new inflection::dialog::SpeakableString((npc((const ::inflection::dialog::NumberConcept*)thisObject)->getAsSpokenWords()));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString* inum_asSpokenWordsVariantCopy(const IDNumberConcept* thisObject, const char16_t* variant, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*) new inflection::dialog::SpeakableString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->asSpokenWords(npc(variant)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString* inum_asDigitsCopy(const IDNumberConcept* thisObject, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*) new inflection::dialog::SpeakableString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->getAsDigits());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI int32_t inum_asOrdinalDigits(const IDNumberConcept* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::copyString(dest, destCapacity, npc((const ::inflection::dialog::NumberConcept*)thisObject)->getAsOrdinalDigits(), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}

INFLECTION_CAPI int32_t inum_asOrginalDigitsVariant(const IDNumberConcept* thisObject, const char16_t* variant, char16_t* dest, int32_t destCapacity, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::copyString(dest, destCapacity, npc((const ::inflection::dialog::NumberConcept*)thisObject)->asDigits(npc(variant)), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}

INFLECTION_CAPI IDNumberConcept* inum_createFromInt64(int64_t value, const char* language, const char* regionLocale, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDNumberConcept*)new ::inflection::dialog::NumberConcept(value, inflection::util::ULocale(npc(language)), inflection::util::ULocale(npc(regionLocale)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDNumberConcept* inum_createFromDouble(double value, const char* language, const char* regionLocale, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDNumberConcept*)new ::inflection::dialog::NumberConcept(value, inflection::util::ULocale(npc(language)), inflection::util::ULocale(npc(regionLocale)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDNumberConcept* inum_clone(const IDNumberConcept* original, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDNumberConcept*)new ::inflection::dialog::NumberConcept(*npc((const inflection::dialog::NumberConcept*) original));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI void inum_destroy(IDNumberConcept *thisObject) {
    delete (::inflection::dialog::NumberConcept*) thisObject;
}

