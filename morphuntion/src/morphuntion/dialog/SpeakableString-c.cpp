/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/dialog/SpeakableString.h>

#include <morphuntion/dialog/SpeakableString.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/CFUtils.hpp>
#include <morphuntion/npc.hpp>

MORPHUNTION_CAPI CFStringRef
mss_createPrintCopy(MDSpeakableString* thisObject, CFErrorRef* error)
{
    try {
        return morphuntion::util::CFUtils::to_CFString(npc((::morphuntion::dialog::SpeakableString*)thisObject)->getPrint());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI CFStringRef
mss_createSpeakCopy(MDSpeakableString* thisObject, CFErrorRef* error)
{
    try {
        return morphuntion::util::CFUtils::to_CFString(npc((::morphuntion::dialog::SpeakableString*)thisObject)->getSpeak());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI bool
mss_speakEqualsPrint(MDSpeakableString* thisObject, CFErrorRef* error)
{
    try {
        return (npc((::morphuntion::dialog::SpeakableString*)thisObject)->speakEqualsPrint());
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return false;
}

MORPHUNTION_CAPI bool
mss_contains(MDSpeakableString* thisObject, CFStringRef s, CFErrorRef* error)
{
    try {
        return (npc((::morphuntion::dialog::SpeakableString*)thisObject)->contains(::morphuntion::util::CFUtils::to_u16string(s)));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return false;
}

MORPHUNTION_CAPI MDSpeakableString*
mss_create(CFStringRef print, CFErrorRef* error)
{
    try {
        return (MDSpeakableString*)new ::morphuntion::dialog::SpeakableString(::morphuntion::util::CFUtils::to_u16string(print));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI MDSpeakableString*
mss_createPrintSpeak(CFStringRef print, CFStringRef speak, CFErrorRef* error)
{
    try {
        return (MDSpeakableString*)new ::morphuntion::dialog::SpeakableString(::morphuntion::util::CFUtils::to_u16string(print),::morphuntion::util::CFUtils::to_u16string(speak));
    }
    catch (const ::std::exception& e) {
        morphuntion::util::CFUtils::convert(e, error);
    }
    return nullptr;
}

MORPHUNTION_CAPI void
mss_destroy(MDSpeakableString* thisObject)
{
    delete (::morphuntion::dialog::SpeakableString*)thisObject;
}

