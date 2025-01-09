/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFArray.h>
#include <unicode/utypes.h>
#include <exception>
#include <string>
#include <string_view>
#include <vector>

namespace util {
class TypeConversionUtils final
{
public:
    static CFStringRef to_CFString(std::u16string_view str);
    static CFStringRef to_CFString(std::string_view str);
    static ::std::u16string to_u16string(CFStringRef str);
    static ::std::string to_string(CFStringRef str);
    static CFArrayRef to_CFArray(const std::vector<::std::u16string> &array);
    static std::vector<std::u16string> to_u16stringVector(CFArrayRef cfArray);
    TypeConversionUtils() = delete;
};
}
