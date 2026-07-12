/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/lang/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <icu4cxx/fwd.hpp>
#include <string>

/**
 * @brief Utility class to filter strings.
 */
class INFLECTION_INTERNAL_API inflection::lang::StringFilterUtil final
{
public:
    /**
     * Returns a set of characters that are frequently used in a given locale.
     */
    static const ::icu4cxx::UnicodeSet& getExemplarCharacters(const ::inflection::util::ULocale& locale);

public:
    /**
     * Returns true when the string contains any characters are from a script that normally does not use whitespace to separate words.
     */
    static bool hasNonWhitespaceSeparableCharacter(::std::u16string_view str);
    /**
     * Returns true when the first character of the string contains is from a script that normally does not use whitespace to separate words.
     */
    static bool isFirstNonWhitespaceSeparableCharacter(::std::u16string_view str);

private:
    StringFilterUtil() = delete;

public:
    /**
     * A set of all characters in the Latin script.
     */
    static const ::icu4cxx::UnicodeSet& LATIN_SCRIPT();
};
