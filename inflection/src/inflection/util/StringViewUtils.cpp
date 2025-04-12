/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/StringViewUtils.hpp>

#include <inflection/util/ULocale.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <unicode/uchar.h>
#include <unicode/ustring.h>

namespace inflection::util {

std::u16string* StringViewUtils::replace(std::u16string* dest, std::u16string_view str, std::u16string_view pattern, std::u16string_view replace)
{
    size_t lastEndPos = 0;
    size_t startPos = 0;
    if (str.length() >= npc(dest)->capacity() && pattern.length() <= replace.length()) {
        // We aren't shrinking the string. At least try to allocate enough to avoid allocating a lot.
        npc(dest)->reserve(str.length());
    }
    while ((startPos = str.find(pattern, startPos)) != std::u16string::npos) {
        dest->append(str.begin() + lastEndPos, str.begin() + startPos);
        dest->append(replace.begin(), replace.end());
        lastEndPos = startPos + pattern.length();
        startPos = lastEndPos; // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
    dest->append(str.begin() + lastEndPos, str.end());
    return dest;
}

::std::u16string_view StringViewUtils::trim(std::u16string_view string)
{
    int32_t origLen = int32_t(string.length());
    int32_t len = origLen;
    int32_t st = 0;

    while ((st < len) && (string[st] <= u' ')) {
        st++;
    }
    while ((st < len) && (string[len - 1] <= u' ')) {
        len--;
    }
    return string.substr(st, len - st);
}

bool StringViewUtils::isAllLowerCase(std::u16string_view string)
{
    if (!string.empty()) {
        int32_t size = int32_t(string.length());
        char32_t cp;
        for (int32_t i = 0; i < size; i += U16_LENGTH(cp)) {
            cp = codePointAt(string, i);
            if (cp != (char32_t)u_tolower(cp)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

bool StringViewUtils::isAllUpperCase(std::u16string_view string)
{
    if (!string.empty()) {
        int32_t size = int32_t(string.length());
        char32_t cp;
        for (int32_t i = 0; i < size; i += U16_LENGTH(cp)) {
            cp = codePointAt(string, i);
            if (cp != char32_t(u_toupper(cp))) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

std::u16string StringViewUtils::capitalizeFirst(std::u16string_view string, const ULocale &locale)
{
    auto const strlen = string.length();
    if (strlen > 1) {
        auto const firstCodePoint = codePointAt(string, 0);
        if (!(bool)u_isupper(firstCodePoint)) {
            ::std::u16string dest;
            StringUtils::appendCodePoint(&dest, firstCodePoint);
            uppercase(&dest, dest, locale);
            auto firstCodePointLen = U16_LENGTH(firstCodePoint);
            dest.append(string.data() + firstCodePointLen, string.data() + string.length());
            return dest;
        }
    } else if (strlen == 1) {
        ::std::u16string dest;
        uppercase(&dest, string, locale);
        return dest;
    }
    return std::u16string(string.data(), string.data() + string.size());
}

::std::u16string* StringViewUtils::uppercase(::std::u16string* dest, std::u16string_view src, const ULocale& locale)
{
    int32_t srcLen = int32_t(src.length());
    npc(dest)->resize(srcLen, 0); // Typically, we're not changing the size.
    auto status = U_ZERO_ERROR;
    int32_t length = u_strToUpper((UChar *)&(*npc(dest))[0], srcLen, (const UChar *)src.data(), srcLen, locale.getName().c_str(), &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        npc(dest)->resize(length, 0);
        status = U_ZERO_ERROR;
        length = u_strToUpper((UChar *)&(*npc(dest))[0], length, (const UChar *)src.data(), srcLen, locale.getName().c_str(), &status);
    }
    ::inflection::exception::ICUException::throwOnFailure(status);
    if (length != int32_t(npc(dest)->size())) {
        // shrink to fit
        npc(dest)->resize(length, 0);
    }
    return dest;
}

::std::u16string* StringViewUtils::lowercase(::std::u16string* dest, std::u16string_view src, const ULocale& locale)
{
    int32_t srcLen = int32_t(src.length());
    npc(dest)->resize(srcLen, 0); // Typically, we're not changing the size.
    auto status = U_ZERO_ERROR;
    int32_t length = u_strToLower((UChar *)&(*npc(dest))[0], srcLen, (const UChar *)src.data(), srcLen, locale.getName().c_str(), &status);
    if (status == U_BUFFER_OVERFLOW_ERROR) {
        npc(dest)->resize(length, 0);
        status = U_ZERO_ERROR;
        length = u_strToLower((UChar *)&(*npc(dest))[0], length, (const UChar *)src.data(), srcLen, locale.getName().c_str(), &status);
    }
    ::inflection::exception::ICUException::throwOnFailure(status);
    if (length != int32_t(npc(dest)->size())) {
        // shrink to fit
        npc(dest)->resize(length, 0);
    }
    return dest;
}

std::string* StringViewUtils::convert(std::string* dest, std::u16string_view src)
{
    // We aren't using std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> because it's not thread safe and deprecated in C++17.

    // Clear the buffer
    npc(dest)->clear();
    int32_t length = int32_t(src.length());
    int32_t destLength = 0;
    int32_t capacity = (int32_t)dest->capacity();
    char buffer[U8_MAX_LENGTH + 1] = {0};
    UChar32 ch = 0;
    int32_t idx = 0;
    auto srcChars = src.data();
    while (idx < length) {
        U16_NEXT(srcChars, idx, length, ch);
        int32_t idx8 = 0;
        // Technically this isn't unsafe because we allocated the size of the maximum UTF-8 code units per code point.
        U8_APPEND_UNSAFE(buffer, idx8, ch);
        destLength += idx8;
        if (destLength >= capacity) {
            // Assume that the current character represents the same script for the rest of the string.
            dest->reserve((length - idx + 1) * idx8);
            capacity = (int32_t)dest->capacity();
        }
        dest->append(buffer, idx8);
    }

    return dest;
}

std::u16string* StringViewUtils::convert(std::u16string* dest, std::string_view src)
{
    // We aren't using std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> because it's not thread safe and deprecated in C++17.

    int32_t srcLength = int32_t(src.length());
    int32_t destCapacity = srcLength; // It's only going to be the same size or less.
    int32_t desiredCapacity = destCapacity;
    auto status = U_ZERO_ERROR;

    npc(dest)->resize(destCapacity);
    u_strFromUTF8WithSub((UChar *)dest->data(), destCapacity, &desiredCapacity, src.data(), srcLength, U_SENTINEL, nullptr, &status);
    // This normally shouldn't fail. The buffer should never get bigger or the UTF-8 is malformed.
    exception::ICUException::throwOnFailure(status);
    if (destCapacity != desiredCapacity) {
        npc(dest)->resize(desiredCapacity);
    }

    return dest;
}

std::string
StringViewUtils::to_string(std::u16string_view src) {
    std::string dest;
    return *convert(&dest, src);
}

std::u16string
StringViewUtils::to_u16string(std::string_view src) {
    std::u16string dest;
    return *convert(&dest, src);
}

char32_t
StringViewUtils::codePointAt(std::u16string_view str, int32_t idx)
{
    int32_t length = int32_t(str.length());
    if (idx < 0 || length <= idx) {
        throw ::inflection::exception::IndexOutOfBoundsException(idx);
    }
    char32_t cp;
    U16_GET(str, 0, idx, length, cp);
    return cp;
}

::std::vector<::std::u16string>
StringViewUtils::split(std::u16string_view str, std::u16string_view delimiter)
{
    ::std::vector<::std::u16string> results;
    ::std::size_t lastIdx = 0;
    ::std::size_t foundIdx;
    while ((foundIdx = str.find(delimiter, lastIdx)) != ::std::u16string::npos) {
        results.emplace_back(str.data() + lastIdx, str.data() + foundIdx);
        lastIdx = foundIdx + delimiter.length();
    }
    // One final one, including when it's not found.
    results.emplace_back(str.data() + lastIdx, str.data() + str.length());
    return results;
}

::std::u16string
StringViewUtils::join(const ::std::vector<::std::u16string>& strings, std::u16string_view delimiter)
{
    ::std::u16string result;
    for (const auto& string : strings) {
        if (!result.empty()) {
            result.append(delimiter.data(), delimiter.size());
        }
        result.append(string);
    }
    return result;
}

char16_t *StringViewUtils::strdup(const char16_t *str) {
    auto len = u_strlen((const UChar *)str) + 1;
    auto result = new char16_t[len];
    u_memcpy((UChar *)result, (const UChar *)str, len);
    return result;
}

} // namespace inflection::util
