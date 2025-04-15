/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <string>
#include <vector>

namespace inflection::util {

/**
 * @brief Provides utility functions for use with <code>std::u16string_view</code> objects.
 * @details This class contains many static functions that are very useful for manipulation, combination, duplicating,
 * and converting of <code>std::u16string</code> objects.
 */
class INFLECTION_INTERNAL_API StringViewUtils {
public:
    /**
     * Attempts to replace **all** instances of _pattern_ in _str_ with _replace_. The resulting string is copied into
     * _dest_, effectively replacing any characters in _dest_ where the replacement overlaps with. This function will
     * not shrink _dest_ to match the replacement, however it will grow it as much as needed.
     *
     * @param dest The destination string to place the replacement string into.
     * @param str The source string.
     * @param pattern The pattern to replace.
     * @param replace The replacement string to replace all instances of pattern in the source string with.
     * @return A pointer to _dest_.
     */
    static ::std::u16string* replace(::std::u16string* dest, std::u16string_view str, std::u16string_view pattern, std::u16string_view replace);
    /**
     * Trims whitespace from the front and back of the string.
     *
     * @param string The string to trim.
     * @return A copy of the input string with whitespace trimmed off the ends.
     */
    static ::std::u16string_view trim(std::u16string_view string);
    /**
     * Returns True if the input string consists of only lowercase characters.
     *
     * @param string The input string.
     * @return True if the input string consists of only lowercase characters.
     */
    static bool isAllLowerCase(std::u16string_view string);
    /**
     * Returns True if the input string consists of only uppercase characters.
     *
     * @param string The input string.
     * @return True if the input string consists of only uppercase characters.
     */
    static bool isAllUpperCase(std::u16string_view string);
    /**
     * Returns a copy of the input string with the first character capitalized in the specified locale.
     *
     * @param string The input string.
     * @param locale The locale to perform capitalization with.
     * @return A copy of the input string with the first character capitalized in the specified locale.
     */
    static ::std::u16string capitalizeFirst(std::u16string_view string, const ::inflection::util::ULocale& locale);
    /**
     * Creates a capitalized copy of the input string in the specified locale and places it into _dest_. This function
     * will shrink or grow _dest_ to fit the capitalized copy exactly.
     *
     * @param dest The destination to store the capitalized copy in.
     * @param src The source string to capitalize.
     * @param locale The locale to perform capitalization in.
     * @return A pointer to the destination string.
     */
    static ::std::u16string* uppercase(::std::u16string* dest, std::u16string_view src, const ::inflection::util::ULocale& locale);
    /**
     * Creates a lowercase copy of the input string in the specified locale and places it into _dest_. This function
     * will shrink or grow _dest_ to fit the lowercase copy exactly.
     *
     * @param dest The destination to store the lowercase copy in.
     * @param src The source string to lowercase.
     * @param locale The locale to perform lowercasing in.
     * @return A pointer to the destination string.
     */
    static ::std::u16string* lowercase(::std::u16string* dest, std::u16string_view src, const ::inflection::util::ULocale& locale);

    /**
     * Converts a <code>std::u16string</code> into a <code>std::string</code>.
     *
     * @param dest The destination <code>std::string</code> to place the conversion in.
     * @param src The source <code>std::u16string</code> to convert.
     * @return A pointer to the destination <code>std::string</code>.
     */
    static ::std::string* convert(::std::string* dest, std::u16string_view src);
    /**
     * Converts a <code>std::string</code> into a <code>std::u16string</code>.
     *
     * @param dest The destination <code>std::u16string</code> to place the conversion in.
     * @param src The source <code>std::string</code> to convert.
     * @return A pointer to the destination <code>std::u16string</code>.
     */
    static ::std::u16string* convert(::std::u16string* dest, std::string_view src);
    /**
     * Converts and creates a copy of a <code>std::u16string</code> to a <code>std::string</code>.
     *
     * @param i The input string to convert.
     * @return The converted copy of the input string.
     */
    static ::std::string to_string(std::u16string_view i);
    /**
     * Converts and creates a copy of a <code>std::string</code> to a <code>std::u16string</code>.
     *
     * @param i The input string to convert.
     * @return The converted copy of the input string.
     */
    static ::std::u16string to_u16string(::std::string_view i);
    /**
     * Returns the codepoint at the specified index in the input string.
     *
     * @param str The input string.
     * @param idx The index.
     * @return The codepoint at the specified index in the input string.
     */
    static char32_t codePointAt(std::u16string_view str, int32_t idx);
    /**
     * Splits the input string along each delimiter within the input string, creating an array with each resulting string
     * (minus the delimiters).
     *
     * @param str The input string to split.
     * @param delimiter The delimiter to split on.
     * @return An array as a <code>std::vector<std::u16string></code> containing the result of the split.
     */
    static ::std::vector<::std::u16string> split(std::u16string_view str, std::u16string_view delimiter);
    /**
     * Joins an array of strings, using _delimiter_ as the delimiter to join with. The output is a string with all
     * strings in input joined together with _delimiter_ in-between each.
     *
     * @param strings The input strings to join together.
     * @param delimiter The delimiter to use.
     * @return A string created from _strings_ with _delimiter_ in-between each.
     */
    static ::std::u16string join(const ::std::vector<::std::u16string>& strings, std::u16string_view delimiter);

    /**
     * Duplicates the input string.
     *
     * The returned pointer is _unmanaged_ and must manually be *deleted*. The use of a smart pointer is recommended.
     *
     * @param str The input string.
     * @return The duplicated string.
     */
    static char16_t* strdup(const char16_t *str);

private:
    StringViewUtils() = delete;
};

}
