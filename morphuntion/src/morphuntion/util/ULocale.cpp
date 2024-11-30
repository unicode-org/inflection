/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/util/Logger.hpp>
#include <morphuntion/util/LoggerConfig.hpp>
#include <morphuntion/util/StringViewUtils.hpp>
#include <unicode/uloc.h>

namespace morphuntion::util {


ULocale::~ULocale()
{
}

static constexpr char UNDERSCORE = '_';
static constexpr char AT_SIGN = '@';
static constexpr char EQUALS_SIGN = '=';

static inline char getDelimiter(std::string_view str)
{
    if (str.find(EQUALS_SIGN) == std::string::npos) {
        return UNDERSCORE;
    }
    else {
        return AT_SIGN;
    }
}

ULocale::ULocale(std::string_view newLanguage,
                 std::string_view newCountry,
                 std::string_view newVariant,
                 std::string_view newKeywords)
{
    int32_t newLanguageLength = int32_t(newLanguage.length());
    int32_t approximateSize = int32_t(newLanguageLength + newCountry.length() + newVariant.length() + newKeywords.length());
    approximateSize += int8_t(approximateSize != newLanguageLength); // Usually add 1 for the underscore if needed.
    fullName.reserve(approximateSize);
    fullName += newLanguage;
    if (newCountry.length() > 0) {
        fullName += getDelimiter(newCountry);
        fullName += newCountry;
    }
    if (newVariant.length() > 0) {
        fullName += getDelimiter(newVariant);
        fullName += newVariant;
    }
    if (newKeywords.length() > 0) {
        if (newCountry.length() == 0) {
            fullName += UNDERSCORE;
        }
        fullName += getDelimiter(newKeywords);
        fullName += newKeywords;
    }

    char fullNameBuffer[ULOC_FULLNAME_CAPACITY] = {0};
    if (fullName.length() < ULOC_FULLNAME_CAPACITY) {
        auto status = U_ZERO_ERROR;
        if (fullName.length() > 0) {
            uloc_canonicalize(fullName.c_str(), fullNameBuffer, ULOC_FULLNAME_CAPACITY, &status);
            if (status == U_ZERO_ERROR) {
                fullName = fullNameBuffer;
            }
        }
        uloc_getLanguage(fullName.c_str(), fullNameBuffer, ULOC_FULLNAME_CAPACITY, &status);
        if (status == U_ZERO_ERROR) {
            language = fullNameBuffer;
        }
        uloc_getScript(fullName.c_str(), fullNameBuffer, ULOC_FULLNAME_CAPACITY, &status);
        if (status == U_ZERO_ERROR) {
            script = fullNameBuffer;
        }
        uloc_getCountry(fullName.c_str(), fullNameBuffer, ULOC_FULLNAME_CAPACITY, &status);
        if (status == U_ZERO_ERROR) {
            country = fullNameBuffer;
        }
        auto atStart = fullName.find(AT_SIGN);
        if (atStart == std::string::npos) {
            uloc_getVariant(fullName.c_str(), fullNameBuffer, ULOC_FULLNAME_CAPACITY, &status);
            if (status == U_ZERO_ERROR) {
                variant = fullNameBuffer;
            }
        }
        else {
            variant.assign(fullName, atStart + 1);
        }
        if (status == U_ZERO_ERROR) {
            return;
        }
    }
    if (LoggerConfig::isErrorEnabled()) {
        Logger::error(u"Invalid locale " + StringViewUtils::to_u16string(fullName));
    }
    // Bad state
    *this = ULocale("", "", "", "");
}

ULocale::ULocale(const ULocale &other)
{
    *this = other;
}

ULocale &ULocale::operator=(const ULocale &other)
{
    if (this == &other) {
        return *this;
    }

    language = other.language;
    script = other.script;
    country = other.country;
    variant = other.variant;
    fullName = other.fullName;
    return *this;
}

ULocale ULocale::getFallback() const
{
    auto extStart = fullName.find(AT_SIGN);
    if (extStart == std::string::npos) {
        extStart = fullName.length();
    }
    else {
        return ULocale(fullName.substr(0, extStart));
    }
    auto last = fullName.rfind('_', extStart);
    if (last == std::string::npos) {
        last = 0;
    } else {
        // truncate empty segment
        while (last > 0) {
            if (fullName[last - 1] != '_') {
                break;
            }
            last--;
        }
    }
    return ULocale(fullName.substr(0, last) + fullName.substr(extStart));
}

::std::u16string ULocale::toString() const
{
    return StringViewUtils::to_u16string(fullName);
}

std::size_t ULocale::operator()(const ULocale& locale) const noexcept
{
    return std::hash<std::string>()(locale.fullName);
}

} // namespace morphuntion::util
