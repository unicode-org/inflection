/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/dialog/SpeakableConcept.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/util/ULocale.hpp>
#include <icu4cxx/fwd.hpp>
#include <unicode/unum.h>
#include <string>

/**
 * @brief Provides representation of numerics as lexical tokens, such as ordinals or cardinals.
 * @details An object that provides representation of numerics.
 *
 * As with the other concepts within Siri, NumberConcept provides utility functions, grammatical rules, and
 * lexical representation for a real-world or linguistic _concept_. In this case: generic _numerals_.
 */
class INFLECTION_CLASS_API inflection::dialog::NumberConcept
    : public virtual SpeakableConcept
{
public:
    /// @cond
    typedef ::inflection::dialog::SpeakableConcept super;
    /// @endcond

private:
    struct Number {
        union {
            int64_t int64Val;
            double doubleVal;
        } value;
        enum {
            INT64,
            DOUBLE
        } type;
    };

private:
    Number value {  };
    ::inflection::util::ULocale language;
    ::inflection::util::ULocale regionLocale;
    int32_t maximumFractionDigits { -1 };
    int32_t minimumFractionDigits { -1 };
    int32_t maximumIntegerDigits { -1 };
    int32_t minimumIntegerDigits { -1 };
    int32_t groupingUsed { -1 };
    int32_t roundingMode { -1 };

private:
    static int64_t longValue(const ::inflection::dialog::NumberConcept::Number& value);
    static double doubleValue(const ::inflection::dialog::NumberConcept::Number& value);
    void copySettings(const NumberConcept& original);

public:
    /**
     * Return the absolute value of this NumberConcept.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* getAbsolute() const;

private:
//    ::com::ibm::icu::math::BigDecimal* toBigDecimal() const;
//    static ::inflection::dialog::NumberConcept::Number toSmallestPrimitiveType(const ::inflection::dialog::NumberConcept::Number& value);
//    Number getRoundedNumber() const;

public:
//    virtual NumberConcept* getRoundedValue() const;

private:
    NumberConcept* getMutableVersion() const;

public:
    /**
     * Set the minimum number of digits to show after the decimal point for a new NumberConcept.
     * For example: 12 turns into 12.00 when set to 2.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* minFractionDigits(int32_t min) const;

    /**
     * Set the maximum number of digits to show after the decimal point for a new NumberConcept.
     * For example: 12.3456 turns into 12.35 when set to 2.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* maxFractionDigits(int32_t max) const;

    /**
     * For values greater than 1.0, always have given fractional digits.
     * For numbers between 1.0 and 0.0, always have at least given non-zero digits, no matter how many leading zeros are needed.
     * Omit the trailing zeros of the fractional part.
     */
    virtual NumberConcept* maxFractionDigitsAboveOne(int32_t given) const;

    /**
     * Get the maximum number of digits to show after the decimal point
     */
    virtual int32_t getMaximumFractionDigits() const;

    /**
     * Set the minimum number of digits to show before the decimal point for a new NumberConcept.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* minIntegerDigits(int32_t min) const;

    /**
     * Set the maximum number of digits to show before the decimal point for a new NumberConcept.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* maxIntegerDigits(int32_t max) const;

    /**
     * Is grouping shown? For example: 1,234.56 turns into 1234.56 when grouping is false.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* grouping(bool grouping) const;

    /**
     * Round away from zero. Use maxFractionDigits to round at a specific place after the decimal point.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* roundUp() const;

    /**
     * Round toward zero. Use maxFractionDigits to round at a specific place after the decimal point.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* roundDown() const;

    /**
     * Round to closest number, preferring away from zero if halfway in between.
     * Use maxFractionDigits to round at a specific place after the decimal point.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* roundHalfUp() const;

    /**
     * Round to closest number, preferring toward zero if halfway in between.
     * Use maxFractionDigits to round at a specific place after the decimal point.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* roundHalfDown() const;

    /**
     * Rounds any fraction towards positive infinity.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* roundCeiling() const;

    /**
     * Rounds any fraction towards negative infinity.
     * @return A new NumberConcept.
     */
    virtual NumberConcept* roundFloor() const;

    /**
     * Returns true when the number is negative.
     */
    virtual bool getIsNegative() const;

private:
    static bool hasFraction(const Number& value);

public:
    /**
     * Returns true when the value can not be represented as an integer without rounding.
     */
    virtual bool hasFraction() const;

private:
    NumberConcept* round(int32_t roundMode) const;

public:
    /**
     * Returns the language used to format the speakable part of a SpeakableString.
     */
    virtual const ::inflection::util::ULocale& getLanguage() const;

    /**
     * Returns the locale used to format the printable part of a SpeakableString.
     */
    virtual const ::inflection::util::ULocale& getRegionLocale() const;

    /**
     * Return a debuggable form of this NumberConcept.
     */
    ::std::u16string toString() const override;

    /**
     * Return a SpeakableString of this NumberConcept. The behavior is the same as getAsDigits().
     */
    SpeakableString* toSpeakableString() const override;

    /**
     * Format the number with words. The words are based on the language used to construct this object.
     */
    virtual ::std::u16string getAsWords() const;

    /**
     * Format the number with words with the variant rule name. The words are based on the language used to construct this object.
     * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp for details.
     */
    virtual ::std::u16string asWords(const ::std::u16string& variant) const;

    /**
     * Format the number with a printable and speakable form. The print part of the SpeakableString is the same as asDigits().
     * The speak part of the SpeakableString is the same as asWords().
     */
    virtual SpeakableString getAsSpokenWords() const;

    /**
     * Format the number with a printable and speakable form. The print part of the SpeakableString is the same as asDigits().
     * The speak part of the SpeakableString is the same as asWords(variant).
     * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp for details.
     */
    virtual SpeakableString asSpokenWords(const ::std::u16string& variant) const;

    /**
     * Format the number with digits using the region locale. The speak part may contain words or digits for the given language
     * when the language is not the same as the region locale or the TTS system can not properly pronounce the printed form.
     */
    virtual SpeakableString getAsDigits() const;

    /**
     * Format the number as ordinal digits using the language.
     * For example, the number would be formatted as 1st, 2nd and 3rd in English.
     * @see https://st.unicode.org/cldr-apps/numbers.jsp?type=OrdinalRules
     */
    virtual ::std::u16string getAsOrdinalDigits() const;

    /**
     * Format the number as ordinal digits using the language with the requested variant.
     * For example, you can request the Spanish forms for 1º. and 1ª.
     * @param variant Check out the complete list of rule names see https://st.unicode.org/cldr-apps/numbers.jsp?type=OrdinalRules for details.
     */
    virtual ::std::u16string asDigits(const ::std::u16string& variant) const;

private:
    ::icu4cxx::NumberFormat createSimpleNumberFormat(const ::inflection::util::ULocale& locale) const;
    ::icu4cxx::NumberFormat* configure(::icu4cxx::NumberFormat* numberFormat) const;
    ::std::u16string format(UNumberFormatStyle type, const ::std::u16string& variant) const;
    ::icu4cxx::NumberFormat createRuleBasedNumberFormat(UNumberFormatStyle type) const;
    bool isIntegral() const;
    ::std::u16string format(const ::icu4cxx::NumberFormat& rbnf) const;
    static ::std::u16string postProcess(const ::std::u16string& formattedValue);

public:
    /**
     * Returns true when both NumberConcept objects are the same value.
     * Formatting differences are not considered.
     */
    bool operator==(const NumberConcept& o) const;

    /**
     * @return Returns the long value of this NumberConcept cast as an `size_t`.
     */
    std::size_t operator()(const NumberConcept& o) const noexcept;

    /**
     * String compares the order of the NumberConcept objects.
     * Formatting differences are not considered.
     * @param o The object to be compared with this.
     */
    std::partial_ordering operator<=>(const NumberConcept& o) const;

    /**
     * Returns the value of this NumberConcept as a 64-bit number.
     */
    int64_t longValue() const;

    /**
     * Returns the value of this NumberConcept as a double number.
     */
    double doubleValue() const;

    /**
     * Construct a NumberConcept.
     * @param value a 64-bit value
     * @param language This locale is primarily used for the speak property of a SpeakableString.
     * @param regionLocale This locale is primarily used for the print property of a SpeakableString.
     */
    NumberConcept(int64_t value, const ::inflection::util::ULocale& language, const ::inflection::util::ULocale& regionLocale);
    /**
     * Construct a NumberConcept.
     * @param value an IEEE 754 double value.
     * @param language This locale is primarily used for the speak property of a SpeakableString.
     * @param regionLocale This locale is primarily used for the print property of a SpeakableString.
     */
    NumberConcept(double value, const ::inflection::util::ULocale& language, const ::inflection::util::ULocale& regionLocale);
    /**
     * Copy constructor
     */
    NumberConcept(const NumberConcept& original);

    ~NumberConcept() override;
private:
    NumberConcept& operator=(const NumberConcept&) = delete;
};
