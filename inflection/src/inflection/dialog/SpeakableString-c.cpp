/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SpeakableString.h>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI CFStringRef
iss_createPrintCopy(IDSpeakableString* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(npc((::inflection::dialog::SpeakableString*)thisObject)->getPrint());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI CFStringRef
iss_createSpeakCopy(IDSpeakableString* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::to_CFString(npc((::inflection::dialog::SpeakableString*)thisObject)->getSpeak());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI bool
iss_speakEqualsPrint(IDSpeakableString* thisObject, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (npc((::inflection::dialog::SpeakableString*)thisObject)->speakEqualsPrint());
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI bool
iss_contains(IDSpeakableString* thisObject, CFStringRef s, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (npc((::inflection::dialog::SpeakableString*)thisObject)->contains(::inflection::util::TypeConversionUtils::to_u16string(s)));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI IDSpeakableString*
iss_create(CFStringRef print, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*)new ::inflection::dialog::SpeakableString(::inflection::util::TypeConversionUtils::to_u16string(print));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString*
iss_createPrintSpeak(CFStringRef print, CFStringRef speak, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return (IDSpeakableString*)new ::inflection::dialog::SpeakableString(::inflection::util::TypeConversionUtils::to_u16string(print), ::inflection::util::TypeConversionUtils::to_u16string(speak));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI void
iss_destroy(IDSpeakableString* thisObject)
{
    delete (::inflection::dialog::SpeakableString*)thisObject;
}

