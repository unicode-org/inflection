/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/exception/fwd.hpp>
#include <inflection/exception/RuntimeException.hpp>
#include <unicode/utypes.h>

/**
 * @brief Used to indicate an error utilizing the ICU unicode library.
 * @details This type of exception is typically thrown when encoding, decoding, or doing character set conversion
 * while utilizing the underlying <a href="https://icu.unicode.org/">ICU library</a>.
 */
class INFLECTION_CLASS_API inflection::exception::ICUException
    : public inflection::exception::RuntimeException
{
public:
    /// @cond
    typedef ::inflection::exception::RuntimeException super;
    /// @endcond

private:
    UErrorCode status { U_ZERO_ERROR };

public:
    /**
     * Construct an exception for the provided UErrorCode.
     */
    explicit ICUException(UErrorCode status);
    /**
     * Construct an exception with an informative message for debugging purposes for the provided UErrorCode.
     */
    ICUException(UErrorCode status, const std::u16string& message);
    /**
     * Destructor
     */
    ~ICUException() override;

    /**
     * Return the original UErrorCode.
     */
    UErrorCode getError() const;

    /**
     * Throw an exception when the UErrorCode is a failure.
     * @param status The UErrorCode to check
     */
    static void throwOnFailure(UErrorCode status);
    /**
     * Throw an exception when the UErrorCode is a failure.
     * @param status The UErrorCode to check
     * @param message The additional message to return to aid in diagnosing the failure.
     */
    static void throwOnFailure(UErrorCode status, const char16_t* message);
    /**
     * Throw an exception when the UErrorCode is a failure.
     * @param status The UErrorCode to check
     * @param message The additional message to return to aid in diagnosing the failure.
     */
    static void throwOnFailure(UErrorCode status, const std::u16string& message);
};

inline
void
inflection::exception::ICUException::throwOnFailure(UErrorCode status)
{
    if (U_FAILURE(status)) {
        throw ICUException(status);
    }
}

inline
void
inflection::exception::ICUException::throwOnFailure(UErrorCode status, const char16_t* message)
{
    if (U_FAILURE(status)) {
        throw ICUException(status, message);
    }
}

inline
void
inflection::exception::ICUException::throwOnFailure(UErrorCode status, const std::u16string& message)
{
    if (U_FAILURE(status)) {
        throw ICUException(status, message);
    }
}
