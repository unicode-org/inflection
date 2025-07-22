/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/NumberConcept.hpp>

#include <inflection/dialog/SpeakableString.hpp>
#include <inflection/util/StringViewUtils.hpp>
#include <inflection/util/LocaleUtils.hpp>
#include <inflection/util/Logger.hpp>
#include <inflection/util/LoggerConfig.hpp>
#include <inflection/npc.hpp>
#include <icu4cxx/NumberFormat.hpp>
#include <math.h>

namespace inflection::dialog {

NumberConcept::NumberConcept(double value, const ::inflection::util::ULocale& language, const ::inflection::util::ULocale& regionLocale)
    : super()
    , language(language)
    , regionLocale(regionLocale)
{
    this->value.value.doubleVal = value;
    this->value.type = Number::DOUBLE;
}

NumberConcept::NumberConcept(int64_t value, const ::inflection::util::ULocale& language, const ::inflection::util::ULocale& regionLocale)
    : super()
    , language(language)
    , regionLocale(regionLocale)
{
    this->value.value.int64Val = value;
    this->value.type = Number::INT64;
}

NumberConcept::NumberConcept(const NumberConcept& original)
    : super()
    , value(original.value)
    , language(original.language)
    , regionLocale(original.regionLocale)
{
    copySettings(original);
}

NumberConcept::~NumberConcept()
{
}

int64_t NumberConcept::longValue(const NumberConcept::Number& value)
{
    if (value.type == NumberConcept::Number::DOUBLE) {
        return (int64_t)value.value.doubleVal;
    }
    return value.value.int64Val;
}

double NumberConcept::doubleValue(const NumberConcept::Number& value)
{
    if (value.type == NumberConcept::Number::DOUBLE) {
        return value.value.doubleVal;
    }
    return value.value.int64Val;
}

static constexpr int32_t DEFAULT_FRACTION_DIGITS { 3 };
static constexpr int32_t DEFAULT_STRING_DIGITS_SIZE { 20 }; // Can handle a null terminated 9223372036854775807 by default

static std::u16string formatInt64(const ::icu4cxx::NumberFormat& numberFormat, int64_t value)
{
    auto size = DEFAULT_STRING_DIGITS_SIZE;
    auto status = U_ZERO_ERROR;
    ::std::u16string result;
    result.resize(size, 0);
    size = unum_formatInt64(numberFormat.wrappee_, value, (UChar *)&result[0], size, nullptr, &status);
    if (size != int32_t(result.length())) {
        result.resize(size, 0);
    }
    if (U_FAILURE(status)) {
        numberFormat.formatInt64(value, (UChar *)&result[0], size, nullptr);
    }
    if (size != int32_t(result.length())) {
        result.resize(size, 0); // We're making it shorter.
    }
    return result;
}

static std::u16string formatDouble(const ::icu4cxx::NumberFormat& numberFormat, double value)
{
    auto size = DEFAULT_STRING_DIGITS_SIZE;
    auto status = U_ZERO_ERROR;
    ::std::u16string result;
    result.resize(size, 0);
    size = unum_formatDouble(numberFormat.wrappee_, value, (UChar *)&result[0], size, nullptr, &status);
    if (size != int32_t(result.length())) {
        result.resize(size, 0);
    }
    if (U_FAILURE(status)) {
        numberFormat.formatDouble(value, (UChar *)&result[0], size, nullptr);
    }
    if (size != int32_t(result.length())) {
        result.resize(size, 0); // We're making it shorter.
    }
    return result;
}

void NumberConcept::copySettings(const NumberConcept& original)
{
    maximumFractionDigits = original.maximumFractionDigits;
    minimumFractionDigits = original.minimumFractionDigits;
    maximumIntegerDigits = original.maximumIntegerDigits;
    minimumIntegerDigits = original.minimumIntegerDigits;
    groupingUsed = original.groupingUsed;
    roundingMode = original.roundingMode;
}

NumberConcept* NumberConcept::getAbsolute() const
{
    NumberConcept* numConcept;
    if (getIsNegative()) {
        if (isIntegral()) {
            numConcept = new NumberConcept(-longValue(), language, regionLocale);
        }
        else {
            numConcept = new NumberConcept(-doubleValue(), language, regionLocale);
        }
        npc(numConcept)->copySettings(*this);
    }
    else {
        numConcept = getMutableVersion();
    }
    return numConcept;
}

NumberConcept* NumberConcept::getMutableVersion() const
{
    return new NumberConcept(*this);
}

NumberConcept* NumberConcept::minFractionDigits(int32_t min) const
{
    auto numConcept = getMutableVersion();
    npc(numConcept)->minimumFractionDigits = min;
    return numConcept;
}

NumberConcept* NumberConcept::maxFractionDigits(int32_t max) const
{
    auto numConcept = getMutableVersion();
    npc(numConcept)->maximumFractionDigits = max;
    return numConcept;
}

static constexpr double EFFECTIVE_ZERO_THRESHOLD { 1.0E-16 };

NumberConcept* NumberConcept::maxFractionDigitsAboveOne(int32_t given) const
{
    auto adjustedMax = given;
    auto absoluteValue = fabs(doubleValue());
    if (absoluteValue > EFFECTIVE_ZERO_THRESHOLD) {
        auto const digits = ceil(log10(absoluteValue));
        if (digits < 0) {
            adjustedMax = (int32_t)-digits + given;
        }
    }
    return maxFractionDigits(adjustedMax);
}

int32_t NumberConcept::getMaximumFractionDigits() const
{
    return maximumFractionDigits >= 0 ? maximumFractionDigits : DEFAULT_FRACTION_DIGITS;
}

NumberConcept* NumberConcept::minIntegerDigits(int32_t min) const
{
    auto numConcept = getMutableVersion();
    npc(numConcept)->minimumIntegerDigits = min;
    return numConcept;
}

NumberConcept* NumberConcept::maxIntegerDigits(int32_t max) const
{
    auto numConcept = getMutableVersion();
    npc(numConcept)->maximumIntegerDigits = max;
    return numConcept;
}

NumberConcept* NumberConcept::grouping(bool grouping) const
{
    auto numConcept = getMutableVersion();
    npc(numConcept)->groupingUsed = grouping != 0;
    return numConcept;
}

NumberConcept* NumberConcept::roundUp() const
{
    return round(UNUM_ROUND_UP);
}

NumberConcept* NumberConcept::roundDown() const
{
    return round(UNUM_ROUND_DOWN);
}

NumberConcept* NumberConcept::roundHalfUp() const
{
    return round(UNUM_ROUND_HALFUP);
}

NumberConcept* NumberConcept::roundHalfDown() const
{
    return round(UNUM_ROUND_HALFDOWN);
}

NumberConcept* NumberConcept::roundCeiling() const
{
    return round(UNUM_ROUND_CEILING);
}

NumberConcept* NumberConcept::roundFloor() const
{
    return round(UNUM_ROUND_FLOOR);
}

bool NumberConcept::getIsNegative() const
{
    return doubleValue() < 0;
}

bool NumberConcept::hasFraction(const Number& value)
{
    if (value.type == Number::DOUBLE) {
        return value.value.doubleVal != (int64_t)value.value.doubleVal;
    }
    return false;
}

bool NumberConcept::hasFraction() const
{
    return hasFraction(value);
}

NumberConcept* NumberConcept::round(int32_t roundMode) const
{
    auto numConcept = getMutableVersion();
    npc(numConcept)->roundingMode = roundMode;
    return numConcept;
}

const ::inflection::util::ULocale& NumberConcept::getLanguage() const
{
    return language;
}

const ::inflection::util::ULocale& NumberConcept::getRegionLocale() const
{
    return regionLocale;
}

::std::u16string NumberConcept::toString() const
{
    return u"NumberConcept{" + getAsDigits().getPrint()
        + u","
        + language.toString()
        + u","
        + regionLocale.toString()
        + u"}";
}

SpeakableString* NumberConcept::toSpeakableString() const
{
    return new SpeakableString(getAsDigits());
}

::std::u16string NumberConcept::getAsWords() const
{
    return format(UNUM_SPELLOUT, ::std::u16string());
}

::std::u16string NumberConcept::asWords(const ::std::u16string& variant) const
{
    return format(UNUM_SPELLOUT, u"%spellout-" + variant);
}

SpeakableString NumberConcept::getAsSpokenWords() const
{
    auto const printValue = format(createSimpleNumberFormat(regionLocale));
    return SpeakableString(postProcess(printValue), getAsWords());
}

SpeakableString NumberConcept::asSpokenWords(const ::std::u16string& variant) const
{
    auto const printValue = format(createSimpleNumberFormat(regionLocale));
    return SpeakableString(postProcess(printValue), asWords(variant));
}

SpeakableString NumberConcept::getAsDigits() const
{
    auto printVal = format(createSimpleNumberFormat(regionLocale));
    auto speakVal = printVal;
    if (::inflection::util::LocaleUtils::AUSTRIA().getCountry() == language.getCountry()) {
        if (::inflection::util::LocaleUtils::AUSTRIA() == language) {
            speakVal = format(createSimpleNumberFormat(::inflection::util::LocaleUtils::GERMANY()));
        }
    }
    else if (::inflection::util::LocaleUtils::SWITZERLAND_GERMAN().getCountry() == language.getCountry()) {
        if (::inflection::util::LocaleUtils::SWITZERLAND_GERMAN() == language) {
            speakVal = format(createSimpleNumberFormat(::inflection::util::LocaleUtils::GERMANY()));
        }
        else if (::inflection::util::LocaleUtils::SWITZERLAND_FRENCH() == language) {
            if (llabs(longValue()) < 60) {
                speakVal = format(createSimpleNumberFormat(language));
            } else {
                speakVal = getAsWords();
            }
        }
        else if (::inflection::util::LocaleUtils::SWITZERLAND_ITALIAN() == language) {
            speakVal = format(createSimpleNumberFormat(::inflection::util::LocaleUtils::ITALY()));
        }
        else {
            speakVal = format(createSimpleNumberFormat(language));
        }
    }
    else if (::inflection::util::LocaleUtils::BELGIUM_FRENCH().getCountry() == language.getCountry()) {
        if (::inflection::util::LocaleUtils::BELGIUM_FRENCH() == language) {
            if (llabs(longValue()) < 60) {
                speakVal = format(createSimpleNumberFormat(language));
            }
            else {
                speakVal = getAsWords();
            }
        }
        else {
            speakVal = format(createSimpleNumberFormat(language));
        }
    }
    else if (regionLocale != language) {
        speakVal = format(createSimpleNumberFormat(language));
    }
    return SpeakableString(postProcess(printVal), speakVal);
}

::std::u16string NumberConcept::getAsOrdinalDigits() const
{
    return format(UNUM_ORDINAL, u"%digits-ordinal");
}

::std::u16string NumberConcept::asDigits(const ::std::u16string& variant) const
{
    return format(UNUM_ORDINAL, u"%digits-" + variant);
}

icu4cxx::NumberFormat NumberConcept::createSimpleNumberFormat(const ::inflection::util::ULocale& locale) const
{
    ::icu4cxx::NumberFormat retVal(UNUM_DECIMAL, ::std::u16string_view(), locale.getName().c_str(), nullptr);
    configure(&retVal);
    return retVal;
}

icu4cxx::NumberFormat* NumberConcept::configure(::icu4cxx::NumberFormat* numberFormat) const
{
    if (maximumFractionDigits >= 0) {
        npc(numberFormat)->setAttribute(UNUM_MAX_FRACTION_DIGITS, maximumFractionDigits);
    }
    else {
        auto status = U_ZERO_ERROR;
        auto size = unum_getTextAttribute(npc(numberFormat)->wrappee_, UNUM_DEFAULT_RULESET, nullptr, 0, &status);
        if (status == U_BUFFER_OVERFLOW_ERROR && size > 0) {
            // This is the only way to detect if the type is RBNF in C.
            npc(numberFormat)->setAttribute(UNUM_MAX_FRACTION_DIGITS, DEFAULT_FRACTION_DIGITS);
        }
    }
    if (minimumFractionDigits >= 0) {
        npc(numberFormat)->setAttribute(UNUM_MIN_FRACTION_DIGITS, minimumFractionDigits);
    }
    if (maximumIntegerDigits >= 0) {
        npc(numberFormat)->setAttribute(UNUM_MAX_INTEGER_DIGITS, maximumIntegerDigits);
    }
    if (minimumIntegerDigits >= 0) {
        npc(numberFormat)->setAttribute(UNUM_MIN_INTEGER_DIGITS, minimumIntegerDigits);
    }
    if (groupingUsed >= 0) {
        npc(numberFormat)->setAttribute(UNUM_GROUPING_USED, groupingUsed != 0);
    }
    if (roundingMode >= 0) {
        npc(numberFormat)->setAttribute(UNUM_ROUNDING_MODE, roundingMode);
    }
    else {
        auto status = U_ZERO_ERROR;
        auto size = unum_getTextAttribute(npc(numberFormat)->wrappee_, UNUM_DEFAULT_RULESET, nullptr, 0, &status);
        if (status == U_BUFFER_OVERFLOW_ERROR && size > 0) {
            // This is the only way to detect if the type is RBNF in C.
            npc(numberFormat)->setAttribute(UNUM_ROUNDING_MODE, (int32_t)UNUM_ROUND_HALFUP);
        }
    }
    return numberFormat;
}

::std::u16string NumberConcept::format(UNumberFormatStyle type, const ::std::u16string& variant) const
{
    auto rbnf(createRuleBasedNumberFormat(type));
    if (!variant.empty()) {
        try {
            rbnf.setTextAttribute(UNUM_DEFAULT_RULESET, variant);
            if (isIntegral()) {
                return formatInt64(rbnf, longValue());
            }
            return formatDouble(rbnf, doubleValue());
        }
        catch (const inflection::exception::ICUException&) {
            if (::inflection::util::LoggerConfig::isErrorEnabled()) {
                ::inflection::util::Logger::error(u"Invalid variant for " + language.toString()
                       + u": ["
                       + variant
                       + u"]");
            }
        }
    }
    return format(rbnf);
}

icu4cxx::NumberFormat NumberConcept::createRuleBasedNumberFormat(UNumberFormatStyle type) const
{
    ::icu4cxx::NumberFormat rbnf(type, ::std::u16string_view(), language.getName().c_str(), nullptr);
    configure(&rbnf);
    return rbnf;
}

bool NumberConcept::isIntegral() const
{
    return value.type == Number::INT64;
}

std::u16string NumberConcept::format(const ::icu4cxx::NumberFormat& rbnf) const
{
    ::std::u16string formattedValue;
    if (isIntegral()) {
        formattedValue = formatInt64(rbnf, longValue());
    }
    else {
        formattedValue = formatDouble(rbnf, doubleValue());
    }
    return formattedValue;
}

::std::u16string NumberConcept::postProcess(const ::std::u16string& formattedValue)
{
    if (formattedValue.starts_with(u".") && formattedValue.length() > 1) {
        return u"\u200B.\u2060" + formattedValue.substr(1);
    }
    return formattedValue;
}

bool NumberConcept::operator==(const NumberConcept& o) const
{
    return longValue() == o.longValue() && doubleValue() == o.doubleValue();
}

std::size_t NumberConcept::operator()(const NumberConcept& o) const noexcept
{
    return o.longValue();
}

std::partial_ordering NumberConcept::operator<=>(const NumberConcept& o) const
{
    return doubleValue() <=> o.doubleValue();
}

int64_t NumberConcept::longValue() const
{
    return longValue(value);
}

double NumberConcept::doubleValue() const
{
    return doubleValue(value);
}

} // namespace inflection::dialog
