/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <string>
#include <vector>

/**
 * @brief Provides utility functions for use with <code>std::u16string</code> objects.
 * @details This class contains many static functions that are very useful for manipulation, combination, duplicating,
 * and converting of <code>std::u16string</code> objects.
 */
class MORPHUNTION_INTERNAL_API morphuntion::util::StringUtils final
{
public:
    /**
     * Appends the unicode codepoint to the end of a string. This is the same as appending a character to a string,
     * except safer, as this function will make sure the character is either valid UTF16 or is surrounded by valid
     * UTF16 code points.
     *
     * @param dest The destination string to append to.
     * @param codePoint The codepoint to append.
     * @return A pointer to the destination string with the codepoint appended.
     */
    static ::std::u16string* appendCodePoint(::std::u16string* dest, char32_t codePoint);
    /**
     * Reverses the input string in place.
     *
     * @param value The string to reverse.
     * @return The input string, reversed.
     */
    static ::std::u16string* reverse(::std::u16string* value);
    /**
     * Converts a <code>std::u16string</code> into a <code>std::string</code>.
     *
     * @param dest The destination <code>std::string</code> to place the conversion in.
     * @param src The source <code>std::u16string</code> to convert.
     * @return A pointer to the destination <code>std::string</code>.
     */
    static ::std::string* convert(::std::string* dest, const std::u16string& src);
    /**
     * Converts a <code>std::string</code> into a <code>std::u16string</code>.
     *
     * @param dest The destination <code>std::u16string</code> to place the conversion in.
     * @param src The source <code>std::string</code> to convert.
     * @return A pointer to the destination <code>std::u16string</code>.
     */
    static ::std::u16string* convert(::std::u16string* dest, const std::string& src);
    /**
     * Converts and creates a copy of a <code>std::u16string</code> to a <code>std::string</code>.
     *
     * @param i The input string to convert.
     * @return The converted copy of the input string.
     */
    static ::std::string to_string(const ::std::u16string& i);
    /**
     * Converts and creates a copy of a <code>std::string</code> to a <code>std::u16string</code>.
     *
     * @param i The input string to convert.
     * @return The converted copy of the input string.
     */
    static ::std::u16string to_u16string(const ::std::string& i);
    /**
     * Format an <code>int32_t</code> number to a <code>std::u16string</code>.
     *
     * @param i The 64-bit integer value.
     */
    static ::std::u16string to_u16string(int32_t i);
    /**
     * Format an <code>int64_t</code> number to a <code>std::u16string</code>.
     *
     * @param i The 64-bit integer value.
     */
    static ::std::u16string to_u16string(int64_t i);
    /**
     * Format a <code>double</code> number to a <code>std::u16string</code>.
     *
     * @param d The input number.
     */
    static ::std::u16string to_u16string(double d);

private:
    StringUtils() = delete;
};
