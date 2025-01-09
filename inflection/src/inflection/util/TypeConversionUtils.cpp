/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */

#include <inflection/exception/ClassCastException.hpp>
#include <inflection/exception/Exception.hpp>
#include <inflection/exception/FileNotFoundException.hpp>
#include <inflection/exception/ICUException.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/exception/MissingResourceException.hpp>
#include <inflection/exception/NullPointerException.hpp>
#include <inflection/exception/RuntimeException.hpp>
#include <inflection/exception/StringIndexOutOfBoundsException.hpp>
#include <inflection/exception/Throwable.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/util/AutoCFRelease.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <unicode/utf8.h>
#include <inflection/npc.hpp>

namespace inflection::util {

void
TypeConversionUtils::convert(const ::std::exception& e, UErrorCode* status)
{
    if (status != nullptr) {
        if (dynamic_cast<const ::inflection::exception::ClassCastException*>(&e) != nullptr) {
            *status = U_ARGUMENT_TYPE_MISMATCH;
        } else if (dynamic_cast<const ::inflection::exception::FileNotFoundException*>(&e) != nullptr) {
            *status = U_FILE_ACCESS_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::ICUException*>(&e) != nullptr) {
            *status = dynamic_cast<const ::inflection::exception::ICUException&>(e).getError();
        } else if (dynamic_cast<const ::inflection::exception::IllegalArgumentException*>(&e) != nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IllegalStateException*>(&e) != nullptr) {
            *status = U_INVALID_STATE_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IncompatibleVersionException*>(&e) != nullptr) {
            *status = U_INVALID_FORMAT_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IndexOutOfBoundsException*>(&e) != nullptr) {
            *status = U_INDEX_OUTOFBOUNDS_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::InvalidConfigurationException*>(&e) != nullptr) {
            *status = U_INVALID_FORMAT_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IOException*>(&e) != nullptr) {
            *status = U_INVALID_FORMAT_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::MissingResourceException*>(&e) != nullptr) {
            *status = U_MISSING_RESOURCE_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::NullPointerException*>(&e) != nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::StringIndexOutOfBoundsException*>(&e) != nullptr) {
            *status = U_INDEX_OUTOFBOUNDS_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::XMLParseException*>(&e) != nullptr) {
            *status = U_PARSE_ERROR;
        } else {
            // Exception
            // RuntimeException
            // Throwable
            *status = U_INTERNAL_PROGRAM_ERROR;
        }
    }
}

CFStringRef
TypeConversionUtils::to_CFString(std::u16string_view str)
{
    return CFStringCreateWithCharacters(nullptr, (const UniChar *)str.data(), str.length());
}

CFStringRef
TypeConversionUtils::to_CFString(std::string_view str)
{
    return CFStringCreateWithBytes(nullptr, (const UInt8 *)str.data(), str.length(), kCFStringEncodingUTF8, false);
}

::std::u16string
TypeConversionUtils::to_u16string(CFStringRef str)
{
    ::std::u16string retVal;
    auto len = CFStringGetLength(npc(str));
    retVal.resize(len);
    CFStringGetCharacters(npc(str), CFRangeMake(0, len), (UniChar *)&(retVal[0]));
    return retVal;
}

::std::string
TypeConversionUtils::to_string(CFStringRef str)
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

CFArrayRef TypeConversionUtils::to_CFArray(const ::std::vector<::std::u16string> &array) {
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

::std::vector<::std::u16string> TypeConversionUtils::to_u16stringVector(CFArrayRef cfArray) {
    ::std::vector<::std::u16string> result;
    CFIndex numMappings = CFArrayGetCount(cfArray);
    for (CFIndex idx = 0; idx < numMappings; idx++) {
        result.push_back(TypeConversionUtils::to_u16string(static_cast<CFStringRef>(CFArrayGetValueAtIndex(cfArray, idx))));
    }
    return result;
}

} // namespace inflection::util
