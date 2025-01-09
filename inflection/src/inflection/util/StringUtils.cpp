/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <unicode/uchar.h>
#include <inflection/npc.hpp>

namespace inflection::util {

::std::u16string* StringUtils::appendCodePoint(::std::u16string* dest, char32_t codePoint) {
    if (U16_LENGTH(codePoint) == 1) {
        npc(dest)->push_back(codePoint);
    } else {
        npc(dest)->push_back(U16_LEAD(codePoint));
        npc(dest)->push_back(U16_TRAIL(codePoint));
    }
    return dest;
}

std::u16string* StringUtils::reverse(std::u16string* value)
{
    auto hasSurrogate = false;
    int32_t n = int32_t(npc(value)->length() - 1);
    for (int32_t j = (n - 1) >> 1; j >= 0; --j) {
        auto temp = (*value)[j];
        auto temp2 = (*value)[n - j];
        if (!hasSurrogate) {
            hasSurrogate = (U_IS_SURROGATE(temp) || U_IS_SURROGATE(temp2));
        }
        (*value)[j] = temp2;
        (*value)[n - j] = temp;
    }
    if (hasSurrogate) {
        for (int32_t i = 0; i < n; i++) {
            auto c2 = (*value)[i];
            if (U_IS_TRAIL(c2)) {
                auto c1 = (*value)[i + 1];
                if (U_IS_LEAD(c1)) {
                    (*value)[i++] = c1;
                    (*value)[i] = c2;
                }
            }
        }
    }
    return value;
}

std::string* StringUtils::convert(std::string* dest, const std::u16string& src) {
    return StringViewUtils::convert(dest, src);
}

std::u16string* StringUtils::convert(std::u16string* dest, const std::string& src) {
    return StringViewUtils::convert(dest, src);
}

std::string StringUtils::to_string(const std::u16string& src) {
    return StringViewUtils::to_string(src);
}

std::u16string StringUtils::to_u16string(const std::string& src) {
    return StringViewUtils::to_u16string(src);
}

std::u16string
StringUtils::to_u16string(int32_t i) {
    return to_u16string((int64_t)i);
}

std::u16string
StringUtils::to_u16string(int64_t i) {
    std::u16string result;
    return *convert(&result, std::to_string(i));
}

std::u16string
StringUtils::to_u16string(double d) {
    std::u16string result;
    return *convert(&result, std::to_string(d));
}

} // namespace inflection::util
