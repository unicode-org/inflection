/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/NumberConcept.h>

#include <inflection/dialog/NumberConcept.hpp>
#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI CFStringRef inum_toStringCopy(const IDNumberConcept* thisObject, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->toString());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI CFStringRef inum_asWordsCopy(const IDNumberConcept* thisObject, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->getAsWords());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI CFStringRef inum_asWordsVariantCopy(const IDNumberConcept* thisObject, CFStringRef variant, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->asWords(inflection::util::TypeConversionUtils::to_u16string(variant)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
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

INFLECTION_CAPI IDSpeakableString* inum_asSpokenWordsVariantCopy(const IDNumberConcept* thisObject, CFStringRef variant, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*) new inflection::dialog::SpeakableString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->asSpokenWords(inflection::util::TypeConversionUtils::to_u16string(variant)));
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

INFLECTION_CAPI CFStringRef inum_asOrdinalDigitsCopy(const IDNumberConcept* thisObject, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->getAsOrdinalDigits());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI CFStringRef inum_asOrginalDigitsVariantCopy(const IDNumberConcept* thisObject, CFStringRef variant, UErrorCode* status) {
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(npc((const ::inflection::dialog::NumberConcept*)thisObject)->asDigits(inflection::util::TypeConversionUtils::to_u16string(variant)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
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

