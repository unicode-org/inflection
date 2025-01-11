/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SpeakableString.h>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/Validate.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI int32_t
iss_getPrint(IDSpeakableString* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::copyString(dest, destCapacity,
                                                                     npc((::inflection::dialog::SpeakableString*)thisObject)->getPrint(), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
}

INFLECTION_CAPI int32_t
iss_getSpeak(IDSpeakableString* thisObject, char16_t* dest, int32_t destCapacity, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return inflection::util::TypeConversionUtils::copyString(dest, destCapacity,
                                                                     npc((::inflection::dialog::SpeakableString*)thisObject)->getSpeak(), status);
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return -1;
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
iss_contains(IDSpeakableString* thisObject, const char16_t* str, int32_t strLen, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            return npc((::inflection::dialog::SpeakableString*)thisObject)->contains(strLen < 0
                                                                                     ? ::std::u16string_view(str)
                                                                                     : ::std::u16string_view(str, strLen));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return false;
}

INFLECTION_CAPI IDSpeakableString*
iss_create(const char16_t* print, int32_t printLen, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(print);
            return (IDSpeakableString*)new ::inflection::dialog::SpeakableString(printLen < 0
                                                                                 ? ::std::u16string_view(print)
                                                                                 : ::std::u16string_view(print, printLen));
        }
        catch (const ::std::exception& e) {
            inflection::util::TypeConversionUtils::convert(e, status);
        }
    }
    return nullptr;
}

INFLECTION_CAPI IDSpeakableString*
iss_createPrintSpeak(const char16_t* print, int32_t printLen, const char16_t* speak, int32_t speakLen, UErrorCode* status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        try {
            inflection::util::Validate::notNull(print);
            inflection::util::Validate::notNull(speak);
            return (IDSpeakableString*)new ::inflection::dialog::SpeakableString(printLen < 0
                                                                                 ? ::std::u16string_view(print)
                                                                                 : ::std::u16string_view(print, printLen),
                                                                                 speakLen < 0
                                                                                 ? ::std::u16string_view(speak)
                                                                                 : ::std::u16string_view(speak, speakLen));
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

