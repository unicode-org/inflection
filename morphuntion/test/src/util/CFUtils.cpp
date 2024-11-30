/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */

#include "util/CFUtils.hpp"
#include <morphuntion/util/ULocale.hpp>
#include <unicode/utf8.h>
#include <morphuntion/npc.hpp>

using namespace morphuntion::util;

namespace util {

void
CFUtils::convert(const ::std::exception& e, CFErrorRef* error)
{
    if (error != nullptr) {
        CFStringRef message = CFStringCreateWithCString(nullptr, e.what(), kCFStringEncodingUTF8);
        *error = CFErrorCreate(nullptr, message, 0, nullptr);
        CFRelease(message);
    }
}

CFStringRef
CFUtils::to_CFString(std::u16string_view str)
{
    return CFStringCreateWithCharacters(nullptr, (const UniChar *)str.data(), str.length());
}

CFStringRef
CFUtils::to_CFString(std::string_view str)
{
    return CFStringCreateWithBytes(nullptr, (const UInt8 *)str.data(), str.length(), kCFStringEncodingUTF8, false);
}

::std::u16string
CFUtils::to_u16string(CFStringRef str)
{
    ::std::u16string retVal;
    auto len = CFStringGetLength(npc(str));
    retVal.resize(len);
    CFStringGetCharacters(npc(str), CFRangeMake(0, len), (UniChar *)&(retVal[0]));
    return retVal;
}

::std::string
CFUtils::to_string(CFStringRef str)
{
    ::std::string retVal;
    auto len = CFStringGetLength(npc(str));
    retVal.resize(len);
    if (CFStringGetBytes(npc(str), CFRangeMake(0, len), kCFStringEncodingUTF8, 0, false, (UInt8 *)&(retVal[0]), len, nullptr) != len)
    {
        auto capacity = len * U8_MAX_LENGTH;
        retVal.resize(capacity);
        CFStringGetBytes(npc(str), CFRangeMake(0, len), kCFStringEncodingUTF8, 0, false, (UInt8 *)&(retVal[0]), capacity, &capacity);
        retVal.resize(capacity);
    }
    return retVal;
}

ULocale CFUtils::convert(CFLocaleRef locale)
{
    CFStringRef localeStrRef = CFLocaleGetIdentifier(npc(locale));
    return {to_string(localeStrRef)};
}

CFLocaleRef CFUtils::convert(const ULocale& locale)
{
    CFStringRef localeName = CFUtils::to_CFString(locale.getName());
    CFLocaleRef localeRef = CFLocaleCreate(nullptr, localeName);
    CFRelease(localeName);
    return localeRef;
}

CFArrayRef CFUtils::to_CFArray(const ::std::vector<::std::u16string> &array) {
    ::std::vector<CFStringRef> result;
    for (const auto &item : array) {
        result.push_back(to_CFString(item));
    }
    auto retVal = CFArrayCreate(nullptr, reinterpret_cast<const void **>(result.data()), (CFIndex) result.size(), &kCFTypeArrayCallBacks);
    for (auto item : result) {
        CFRelease(item);
    }
    return retVal;
}

::std::vector<::std::u16string> CFUtils::to_u16stringVector(CFArrayRef cfArray) {
    ::std::vector<::std::u16string> result;
    CFIndex numMappings = CFArrayGetCount(cfArray);
    for (CFIndex idx = 0; idx < numMappings; idx++) {
        result.push_back(CFUtils::to_u16string(static_cast<CFStringRef>(CFArrayGetValueAtIndex(cfArray, idx))));
    }
    return result;
}

} // namespace morphuntion::util
