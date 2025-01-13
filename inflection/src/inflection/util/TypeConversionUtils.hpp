/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/api.h>
#include <inflection/util/fwd.hpp>
#include <unicode/utypes.h>
#include <exception>
#include <string>
#include <string_view>
#include <vector>

class inflection::util::TypeConversionUtils final
{
public:
    static void convert(const ::std::exception& e, UErrorCode* status);
    static int32_t terminateCharString(char *dest, int32_t destCapacity, int32_t length, UErrorCode *status);
    static int32_t terminateString(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *status);
    static int32_t copyString(char16_t *dest, int32_t destCapacity, std::u16string_view str, UErrorCode *status);
    TypeConversionUtils() = delete;
};
