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
    static ::std::u16string filteredUnaccent(const ::icu4cxx::UnicodeSet& foreignSet, const ::icu4cxx::UnicodeSet& setToRemove, const ::std::u16string& str);

private:
    static const ::icu4cxx::UnicodeSet* getSetSingleton(::icu4cxx::UnicodeSet* set);

public:
    /**
     * Returns a set of characters that are frequently used in a given locale.
     */
    static const ::icu4cxx::UnicodeSet& getExemplarCharacters(const ::inflection::util::ULocale& locale);

public:
    /**
     * Returns the set of characters that can be modified by an NFD normalization and are foreign to a given language.
     */
    static const ::icu4cxx::UnicodeSet& getNfdVolatileForeignExemplarCharacters(const ::inflection::util::ULocale& locale);
    /**
     * Removes all diacritics (accents) from a string that are foreign to a language.
     */
    static ::std::u16string stripNonNativeAccents(const ::inflection::util::ULocale& locale, const ::std::u16string& str);
    /**
     * Removes all diacritics (accents) from a string regardless of language.
     */
    static ::std::u16string* unaccent(::std::u16string* dest, ::std::u16string_view s);
    static bool hasAccents(::std::u16string_view s);
    static bool equalIgnoreCaseAndAllAccents(::std::u16string_view s1, ::std::u16string_view s2, const ::inflection::util::ULocale& locale);
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
    /**
     * A set of all characters in the Cyrillic script.
     */
    static const ::icu4cxx::UnicodeSet& CYRILLIC_SCRIPT();
    /**
     * A set of all characters in the Arabic script.
     */
    static const ::icu4cxx::UnicodeSet& ARABIC_SCRIPT();
    /**
     * A set of all characters in the Hebrew script.
     */
    static const ::icu4cxx::UnicodeSet& HEBREW_SCRIPT();
    /**
     * A set of all characters in the Malayalam script.
     */
    static const ::icu4cxx::UnicodeSet& MALAYALAM_SCRIPT();
    /**
     * A set of all characters in the Han script. The Han script is unified between Chinese, Japanese and Korean.
     */
    static const ::icu4cxx::UnicodeSet& HAN_SCRIPT();
    /**
     * A set of all characters in the Hangul script. The Hangul script is used by Korean.
     */
    static const ::icu4cxx::UnicodeSet& HANGUL_SCRIPT();
private:
    static const ::icu4cxx::UnicodeSet& NON_WHITESPACE_SEPARATED_SCRIPTS();
public:
    /**
     * The set of characters that can be modified by an NFD normalization.
     */
    static const ::icu4cxx::UnicodeSet& NFD_VOLATILE_CHARACTERS();
    /**
     * The set of characters that represent all combining diacritical marks.
     */
    static const ::icu4cxx::UnicodeSet& COMBINING_DIACRITICAL_MARKS();
};
