/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dictionary/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <icu4cxx/fwd.hpp>
#include <string>
#include <vector>

/**
 * @brief Exposes properties of a phrase.
 */
class INFLECTION_INTERNAL_API inflection::dictionary::PhraseProperties final
{
public:
    /**
     * Returns true if the input string starts with a vowel for the specified locale.
     *
     * @param loc The locale of the input string.
     * @param str The input string.
     * @return True if the input string starts with a vowel.
     */
    static bool isStartsWithVowel(const ::inflection::util::ULocale& loc, ::std::u16string_view str);
    /**
     * Returns true if the input string ends with a vowel for the specified locale.
     *
     * @param loc The locale of the input string.
     * @param str The input string.
     * @return True if the input string ends with a vowel.
     */
    static bool isEndsWithVowel(const ::inflection::util::ULocale& loc, ::std::u16string_view str);
    /**
     * Returns true when the matchableSubset UnicodeSet match the first codepoint in the string that is contained in the important set.
     * @param str The string to match against
     * @param importantSet The important characters to consider for matching. For example, if whitespace is excluded, any initial whitespace will be skipped.
     * @param matchableSubset After performing Unicode NFKD normalization on the first code matching the important, any codepoints in this UnicodeSet will cause this function to return true, and false otherwise.
     * @param matchableSubsets When non-empty, the code points after the initial code point must match the characters in these UnicodeSets.
     */
    static bool isStartsWithUnicodeSets(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet, const ::icu4cxx::UnicodeSet& matchableSubset, const ::std::vector<const ::icu4cxx::UnicodeSet*>&/*...*/ matchableSubsets);
    /**
     * Returns true when the matchableSubset UnicodeSet match the last codepoint in the string that is contained in the important set.
     * @param str The string to match against
     * @param importantSet The important characters to consider for matching. For example, if whitespace is excluded, any trailing whitespace will be skipped.
     * @param matchableSubset After performing Unicode NFKD normalization on the last code matching the important, any codepoints in this UnicodeSet will cause this function to return true, and false otherwise.
     * @param matchableSubsets When non-empty, the code points before the last trailing code point must match the characters in these UnicodeSets.
     */
    static bool isEndsWithUnicodeSets(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet, const ::icu4cxx::UnicodeSet& matchableSubset, const ::std::vector<const ::icu4cxx::UnicodeSet*>&/*...*/ matchableSubsets);
    /**
     * Perform Unicode normalization on the string as defined by the ICU normalizer.
     * @see <a href="https://unicode.org/reports/tr15/">Unicode Standard Annex #15: Unicode Normalization Forms</a>
     */
    static ::std::u16string normalize(const icu4cxx::Normalizer2& normalizer, ::std::u16string_view str);

private:
    static int32_t getFirstUsableIndex(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet);
    static int32_t getLastUsableIndex(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet);

private:
    PhraseProperties() = delete;

public:
    static const ::std::u16string& RIEUL_END_TAG();
    /**
     * The default characters that represent a vowel at the start of a word for most languages.
     */
    static const ::icu4cxx::UnicodeSet& DEFAULT_VOWELS_START();

private:
    static const ::icu4cxx::UnicodeSet& FRENCH_VOWELS_START();

public:
    /**
     * The default characters that represent a vowel at the end of a word for most languages.
     */
    static const ::icu4cxx::UnicodeSet& DEFAULT_VOWELS_END();
private:
    static const ::icu4cxx::UnicodeSet& I_LETTER();
    static const ::icu4cxx::UnicodeSet& J_LETTER();
public:
    /**
     * This UnicodeSet contains the characters that should be considered important normally for detecting vowels in phrases, like letters.
     */
    static const ::icu4cxx::UnicodeSet& DEFAULT_MATCHABLE_SET();
    static const ::icu4cxx::UnicodeSet& PUNCTUATION();
    static const ::icu4cxx::UnicodeSet& SINGLE_QUOTES();
};
