/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/api.h>
#include <morphuntion/util/fwd.hpp>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFError.h>
#include <exception>
#include <string>
#include <string_view>
#include <vector>

class morphuntion::util::CFUtils final
{
public:
    static void convert(const ::std::exception& e, CFErrorRef* error);
    static CFStringRef to_CFString(std::u16string_view str);
    static CFStringRef to_CFString(std::string_view str);
    static ::std::u16string to_u16string(CFStringRef str);
    static ::std::string to_string(CFStringRef str);
    static ::morphuntion::util::ULocale convert(CFLocaleRef locale);
    static CFLocaleRef convert(const morphuntion::util::ULocale& locale);
    static CFArrayRef to_CFArray(const std::vector<::std::u16string> &array);
    static std::vector<std::u16string> to_u16stringVector(CFArrayRef cfArray);
    CFUtils() = delete;
};
