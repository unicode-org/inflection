/*
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/NumberConcept.h>

#include <morphuntion/dialog/NumberConcept.hpp>
#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/npc.hpp>


MORPHUNTION_CAPI CFStringRef mnum_toStringCopy(const MDNumberConcept* thisObject, CFErrorRef* error) {
    try {
        return morphuntion::util::CFUtils::to_CFString(npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->toString());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFStringRef mnum_asWordsCopy(const MDNumberConcept* thisObject, CFErrorRef* error) {
    try {
        return morphuntion::util::CFUtils::to_CFString(npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->getAsWords());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFStringRef mnum_asWordsVariantCopy(const MDNumberConcept* thisObject, CFStringRef variant, CFErrorRef* error) {
    try {
        return morphuntion::util::CFUtils::to_CFString(npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->asWords(morphuntion::util::CFUtils::to_u16string(variant)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString* mnum_asSpokenWordsCopy(const MDNumberConcept* thisObject, CFErrorRef* error) {
    try {
        return (MDSpeakableString*) new morphuntion::dialog::SpeakableString((npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->getAsSpokenWords()));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString* mnum_asSpokenWordsVariantCopy(const MDNumberConcept* thisObject, CFStringRef variant, CFErrorRef* error) {
    try {
        return (MDSpeakableString*) new morphuntion::dialog::SpeakableString(npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->asSpokenWords(morphuntion::util::CFUtils::to_u16string(variant)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString* mnum_asDigitsCopy(const MDNumberConcept* thisObject, CFErrorRef* error) {
    try {
        return (MDSpeakableString*) new morphuntion::dialog::SpeakableString(npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->getAsDigits());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFStringRef mnum_asOrdinalDigitsCopy(const MDNumberConcept* thisObject, CFErrorRef* error) {
    try {
        return morphuntion::util::CFUtils::to_CFString(npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->getAsOrdinalDigits());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFStringRef mnum_asOrginalDigitsVariantCopy(const MDNumberConcept* thisObject, CFStringRef variant, CFErrorRef* error) {
    try {
        return morphuntion::util::CFUtils::to_CFString(npc((const ::morphuntion::dialog::NumberConcept*)thisObject)->asDigits(morphuntion::util::CFUtils::to_u16string(variant)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDNumberConcept* mnum_createFromInt64(int64_t value, CFLocaleRef language, CFLocaleRef regionLocale, CFErrorRef* error) {
    try {
        return (MDNumberConcept*)new ::morphuntion::dialog::NumberConcept(value, ::morphuntion::util::CFUtils::convert(language), ::morphuntion::util::CFUtils::convert(regionLocale));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDNumberConcept* mnum_createFromDouble(double value, CFLocaleRef language, CFLocaleRef regionLocale, CFErrorRef* error) {
    try {
        return (MDNumberConcept*)new ::morphuntion::dialog::NumberConcept(value, ::morphuntion::util::CFUtils::convert(language), ::morphuntion::util::CFUtils::convert(regionLocale));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDNumberConcept* mnum_clone(const MDNumberConcept* original, CFErrorRef* error) {
    try {
        return (MDNumberConcept*)new ::morphuntion::dialog::NumberConcept(*npc((const morphuntion::dialog::NumberConcept*) original));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI void mnum_destroy(MDNumberConcept *thisObject) {
    delete (::morphuntion::dialog::NumberConcept*) thisObject;
}

