/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */

#include <inflection/exception/ClassCastException.hpp>
#include <inflection/exception/FileNotFoundException.hpp>
#include <inflection/exception/ICUException.hpp>
#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/exception/IllegalStateException.hpp>
#include <inflection/exception/IncompatibleVersionException.hpp>
#include <inflection/exception/IndexOutOfBoundsException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/exception/IOException.hpp>
#include <inflection/exception/MissingResourceException.hpp>
#include <inflection/exception/NullPointerException.hpp>
#include <inflection/exception/XMLParseException.hpp>
#include <inflection/util/AutoCRelease.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <unicode/ustring.h>

namespace inflection::util {

void
TypeConversionUtils::convert(const ::std::exception& e, UErrorCode* status)
{
    if (status != nullptr) {
        if (dynamic_cast<const ::inflection::exception::ClassCastException*>(&e) != nullptr) {
            *status = U_ARGUMENT_TYPE_MISMATCH;
        } else if (dynamic_cast<const ::inflection::exception::FileNotFoundException*>(&e) != nullptr) {
            *status = U_FILE_ACCESS_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::ICUException*>(&e) != nullptr) {
            *status = dynamic_cast<const ::inflection::exception::ICUException&>(e).getError();
        } else if (dynamic_cast<const ::inflection::exception::IllegalArgumentException*>(&e) != nullptr) {
            *status = U_ILLEGAL_ARGUMENT_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IllegalStateException*>(&e) != nullptr) {
            *status = U_INVALID_STATE_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IncompatibleVersionException*>(&e) != nullptr) {
            *status = U_INVALID_FORMAT_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IndexOutOfBoundsException*>(&e) != nullptr) {
            *status = U_INDEX_OUTOFBOUNDS_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::InvalidConfigurationException*>(&e) != nullptr) {
            *status = U_INVALID_STATE_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::IOException*>(&e) != nullptr) {
            *status = U_INVALID_FORMAT_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::MissingResourceException*>(&e) != nullptr) {
            *status = U_MISSING_RESOURCE_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::NullPointerException*>(&e) != nullptr) {
            *status = U_MEMORY_ALLOCATION_ERROR;
        } else if (dynamic_cast<const ::inflection::exception::XMLParseException*>(&e) != nullptr) {
            *status = U_PARSE_ERROR;
        } else {
            // Exception
            // RuntimeException
            // Throwable
            *status = U_INTERNAL_PROGRAM_ERROR;
        }
    }
}

int32_t TypeConversionUtils::terminateCharString(char *dest, int32_t destCapacity, int32_t length, UErrorCode *status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        /* not a public function, so no complete argument checking */

        if (length < 0) {
            /* assume that the caller handles this */
        } else if (length < destCapacity) {
            /* NUL-terminate the string, the NUL fits */
            dest[length] = 0;
            /* unset the not-terminated warning but leave all others */
            if (*status == U_STRING_NOT_TERMINATED_WARNING) {
                *status = U_ZERO_ERROR;
            }
        } else if (length == destCapacity) {
            /* unable to NUL-terminate, but the string itself fit - set a warning code */
            *status = U_STRING_NOT_TERMINATED_WARNING;
        } else /* length>destCapacity */ {
            /* even the string itself did not fit - set an error code */
            *status = U_BUFFER_OVERFLOW_ERROR;
        }
    }
    return length;
}

int32_t TypeConversionUtils::terminateString(char16_t *dest, int32_t destCapacity, int32_t length, UErrorCode *status)
{
    if (status != nullptr && U_SUCCESS(*status)) {
        /* not a public function, so no complete argument checking */

        if (length < 0) {
            /* assume that the caller handles this */
        } else if (length < destCapacity) {
            /* NUL-terminate the string, the NUL fits */
            dest[length] = 0;
            /* unset the not-terminated warning but leave all others */
            if (*status == U_STRING_NOT_TERMINATED_WARNING) {
                *status = U_ZERO_ERROR;
            }
        } else if (length == destCapacity) {
            /* unable to NUL-terminate, but the string itself fit - set a warning code */
            *status = U_STRING_NOT_TERMINATED_WARNING;
        } else /* length>destCapacity */ {
            /* even the string itself did not fit - set an error code */
            *status = U_BUFFER_OVERFLOW_ERROR;
        }
    }
    return length;
}

int32_t TypeConversionUtils::copyString(char16_t *dest, int32_t destCapacity, std::u16string_view str, UErrorCode *status)
{
    if (status == nullptr || U_FAILURE(*status)) {
        return (int32_t)str.length();
    }
    auto length = int32_t(str.length());
    if (destCapacity > 0 && length <= destCapacity) {
        u_strncpy((UChar*)dest, (const UChar*)str.data(), length);
    }
    return terminateString(dest, destCapacity, length, status);
}

} // namespace inflection::util
