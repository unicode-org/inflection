/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dictionary/PhraseProperties.hpp>

#include <inflection/dictionary/DictionaryMetaData.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/StringUtils.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/UnicodeSetUtils.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/Normalizer2.hpp>

namespace inflection::dictionary {

static constexpr char16_t DEFAULT_VOWEL_STR[] = u"aeiouıæœøаеиоуыэюя\u1161-\u1175";
static constexpr char16_t VOWEL_START_TAG[] = u"vowel-start";
static constexpr char16_t VOWEL_END_TAG[] = u"vowel-end";
static constexpr char16_t CONSONANT_START_TAG[] = u"consonant-start";
static constexpr char16_t CONSONANT_END_TAG[] = u"consonant-end";

const std::u16string& PhraseProperties::RIEUL_END_TAG()
{
    static auto RIEUL_END_TAG_ = new ::std::u16string(u"rieul-end");
    return *npc(RIEUL_END_TAG_);
}

const icu4cxx::UnicodeSet& PhraseProperties::DEFAULT_VOWELS_START()
{
    static auto DEFAULT_VOWELS_START_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::closeOver(
            npc(new ::icu4cxx::UnicodeSet(u"[" + ::std::u16string(DEFAULT_VOWEL_STR) + u"]")), USET_CASE_INSENSITIVE));
    return *npc(DEFAULT_VOWELS_START_);
}

const icu4cxx::UnicodeSet& PhraseProperties::FRENCH_VOWELS_START()
{
    static auto FRENCH_VOWELS_START_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::closeOver(
            npc(new ::icu4cxx::UnicodeSet(u"[h" + ::std::u16string(DEFAULT_VOWEL_STR) + u"]")), USET_CASE_INSENSITIVE));
    return *npc(FRENCH_VOWELS_START_);
}

const icu4cxx::UnicodeSet& PhraseProperties::DEFAULT_VOWELS_END()
{
    static auto DEFAULT_VOWELS_END_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::closeOver(
            npc(new ::icu4cxx::UnicodeSet(u"[y" + ::std::u16string(DEFAULT_VOWEL_STR) + u"]")), USET_CASE_INSENSITIVE));
    return *npc(DEFAULT_VOWELS_END_);
}

const icu4cxx::UnicodeSet& PhraseProperties::I_LETTER()
{
    static auto I_LETTER_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::closeOver(npc(new ::icu4cxx::UnicodeSet(u"[i]")), USET_CASE_INSENSITIVE));
    return *npc(I_LETTER_);
}

const icu4cxx::UnicodeSet& PhraseProperties::J_LETTER()
{
    static auto J_LETTER_ = ::inflection::util::UnicodeSetUtils::freeze(::inflection::util::UnicodeSetUtils::closeOver(npc(new ::icu4cxx::UnicodeSet(u"[j]")), USET_CASE_INSENSITIVE));
    return *npc(J_LETTER_);
}

const icu4cxx::UnicodeSet& PhraseProperties::DEFAULT_MATCHABLE_SET()
{
    static auto DEFAULT_MATCHABLE_SET_ = ::inflection::util::UnicodeSetUtils::freeze(npc(new ::icu4cxx::UnicodeSet(u"[[:Letter:][:Decimal_Number:]]")));
    return *npc(DEFAULT_MATCHABLE_SET_);
}

const ::icu4cxx::UnicodeSet& PhraseProperties::PUNCTUATION()
{
    static auto PUNCTUATION_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[[[:punctuation:][:symbol:]]-[\\&]]"));
    return *npc(PUNCTUATION_);
}

const icu4cxx::UnicodeSet& PhraseProperties::SINGLE_QUOTES()
{
    static auto SINGLE_QUOTES_ = ::inflection::util::UnicodeSetUtils::freeze(new ::icu4cxx::UnicodeSet(u"[ʼ‘’‛׳]"));
    return *npc(SINGLE_QUOTES_);
}

static const icu4cxx::Normalizer2& NFKD()
{
    static auto NFKD_ = new ::icu4cxx::Normalizer2(::icu4cxx::Normalizer2::getNFKDInstance());
    return *npc(NFKD_);
}

bool PhraseProperties::isStartsWithVowel(const ::inflection::util::ULocale& loc, ::std::u16string_view str)
{
    const auto &dictionary = *npc(DictionaryMetaData::createDictionary(loc));
    int64_t vowelProperty = 0;
    int64_t consonantProperty = 0;
    int64_t wordTypes = 0;
    if (dictionary.getCombinedBinaryType(&wordTypes, str) != nullptr) {
        auto knowsProperties = dictionary.getBinaryProperties(&vowelProperty, {VOWEL_START_TAG}) != nullptr;
        knowsProperties = dictionary.getBinaryProperties(&consonantProperty, {CONSONANT_START_TAG}) != nullptr || knowsProperties;
        if (knowsProperties) {
            if ((wordTypes & vowelProperty) != 0 && (wordTypes & consonantProperty) == 0) {
                return true;
            }
            if ((wordTypes & vowelProperty) == 0 && (wordTypes & consonantProperty) != 0) {
                return false;
            }
        }
    }
    const auto& lang = loc.getLanguage();
    if (::inflection::util::LocaleUtils::FRENCH() == lang) {
        if (wordTypes != 0 && str.starts_with(u"H")) {
            // A known word starting with the capital H that doesn't contains vowel-start, which may be a proper noun.
            return false;
        }
        return isStartsWithUnicodeSets(str, DEFAULT_MATCHABLE_SET(), FRENCH_VOWELS_START(), {});
    }
    return isStartsWithUnicodeSets(str, DEFAULT_MATCHABLE_SET(), DEFAULT_VOWELS_START(), {});
}

bool PhraseProperties::isEndsWithVowel(const ::inflection::util::ULocale& loc, ::std::u16string_view str)
{
    const auto& lang = loc.getLanguage();
    auto matchableSubset = PhraseProperties::DEFAULT_VOWELS_END();
    const auto &dictionary = *npc(DictionaryMetaData::createDictionary(loc));
    ::std::u16string strippedStr;
    ::inflection::util::UnicodeSetUtils::removeSetFromString(&strippedStr, PUNCTUATION(), str);
    int64_t vowelProperty = 0;
    int64_t consonantProperty = 0;
    auto knowsProperties = dictionary.getBinaryProperties(&vowelProperty, {VOWEL_END_TAG}) != nullptr;
    knowsProperties = dictionary.getBinaryProperties(&consonantProperty, {CONSONANT_END_TAG, RIEUL_END_TAG()}) != nullptr || knowsProperties;
    if (knowsProperties) {
        int64_t wordTypes = 0;
        if (dictionary.getCombinedBinaryType(&wordTypes, strippedStr) != nullptr) {
            if ((wordTypes & vowelProperty) != 0 && (wordTypes & consonantProperty) == 0) {
                return true;
            }
            if ((wordTypes & vowelProperty) == 0 && (wordTypes & consonantProperty) != 0) {
                return false;
            }
        }
    }
    if (::inflection::util::LocaleUtils::TURKISH().getLanguage() == lang) {
        matchableSubset = DEFAULT_VOWELS_START();
    }
    else if (::inflection::util::LocaleUtils::DUTCH().getLanguage() == lang
        && isEndsWithUnicodeSets(str, DEFAULT_MATCHABLE_SET(), I_LETTER(), {&J_LETTER()}))
    {
        return true;
    }
    return isEndsWithUnicodeSets(str, DEFAULT_MATCHABLE_SET(), matchableSubset, {});
}

::std::u16string PhraseProperties::normalize(const icu4cxx::Normalizer2& normalizer, ::std::u16string_view str)
{
    auto status = U_ZERO_ERROR;
    int32_t length = int32_t(str.length());
    ::std::u16string normalizedResult;
    if (length > 0) {
        normalizedResult.resize(length, 0); // Typically, we're not changing the size.
        length = unorm2_normalize(normalizer.wrappee_, (const UChar *)str.data(), (int32_t)str.length(), (UChar *)&normalizedResult[0], length, &status);
        if (length != int32_t(normalizedResult.length())) {
            normalizedResult.resize(length, 0);
        }
        if (status == U_BUFFER_OVERFLOW_ERROR) {
            status = U_ZERO_ERROR;
            unorm2_normalize(normalizer.wrappee_, (const UChar *)str.data(), (int32_t)str.length(), (UChar *)&normalizedResult[0], length, &status);
        }
        ::inflection::exception::ICUException::throwOnFailure(status);
    }
    return normalizedResult;
}

bool PhraseProperties::isStartsWithUnicodeSets(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet, const ::icu4cxx::UnicodeSet& matchableSubset, const ::std::vector<const ::icu4cxx::UnicodeSet*>&/*...*/ matchableSubsets)
{
    auto index = getFirstUsableIndex(str, importantSet);
    if (index >= 0) {
        if (matchableSubsets.empty()) {
            ::std::u16string codePoint;
            ::inflection::util::StringUtils::appendCodePoint(&codePoint, ::inflection::util::StringViewUtils::codePointAt(str, index));
            auto firstChar = normalize(NFKD(), codePoint);
            return matchableSubset.contains(::inflection::util::StringViewUtils::codePointAt(firstChar, 0));
        } else {
            if (!matchableSubset.contains(::inflection::util::StringViewUtils::codePointAt(str, index))) {
                return false;
            }
            auto iterator = matchableSubsets.cbegin();
            int32_t strLen = int32_t(str.length());
            for (index++; iterator != matchableSubsets.cend() && index < strLen; index++) {
                if (!npc(*(iterator++))->contains(::inflection::util::StringViewUtils::codePointAt(str, index))) {
                    return false;
                }
            }
            if (iterator == matchableSubsets.cend()) {
                return true;
            }
        }
    }
    return false;
}

bool PhraseProperties::isEndsWithUnicodeSets(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet, const ::icu4cxx::UnicodeSet& matchableSubset, const ::std::vector<const ::icu4cxx::UnicodeSet*>&/*...*/ matchableSubsets)
{
    int32_t index = getLastUsableIndex(str, importantSet);
    if (index >= 0) {
        if (matchableSubsets.empty()) {
            ::std::u16string codePoint;
            ::inflection::util::StringUtils::appendCodePoint(&codePoint, ::inflection::util::StringViewUtils::codePointAt(str, index));
            auto lastChar = normalize(NFKD(), codePoint);
            index = getLastUsableIndex(lastChar, importantSet);
            if (index >= 0) {
                return matchableSubset.contains(::inflection::util::StringViewUtils::codePointAt(lastChar, index));
            }
        } else {
            auto iterator = matchableSubsets.crbegin();
            for (; iterator != matchableSubsets.crend() && index >= 0; index--) {
                if (!npc(*(iterator++))->contains(::inflection::util::StringViewUtils::codePointAt(str, index))) {
                    return false;
                }
            }
            if (iterator != matchableSubsets.crend()) {
                return false;
            }
            if (index >= 0 && matchableSubset.contains(::inflection::util::StringViewUtils::codePointAt(str, index))) {
                return true;
            }
        }
    }
    return false;
}

int32_t PhraseProperties::getFirstUsableIndex(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet)
{
    if (!str.empty()) {
        int32_t strLen = int32_t(str.length());
        for (int32_t firstUsableIdx = 0; firstUsableIdx < strLen; firstUsableIdx++) {
            if (importantSet.contains(::inflection::util::StringViewUtils::codePointAt(str, firstUsableIdx))) {
                return firstUsableIdx;
            }
        }
    }
    return -1;
}

int32_t PhraseProperties::getLastUsableIndex(::std::u16string_view str, const ::icu4cxx::UnicodeSet& importantSet)
{
    if (!str.empty()) {
        for (int32_t lastUsableIndex = int32_t(str.length() - 1); lastUsableIndex >= 0; lastUsableIndex--) {
            if (importantSet.contains(::inflection::util::StringViewUtils::codePointAt(str, lastUsableIndex))) {
                return lastUsableIndex;
            }
        }
    }
    return -1;
}

} // namespace inflection::dictionary
