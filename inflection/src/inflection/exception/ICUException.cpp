/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <inflection/exception/ICUException.hpp>
#include <inflection/util/StringViewUtils.hpp>

namespace inflection::exception {

ICUException::ICUException(UErrorCode status)
    : super(::inflection::util::StringViewUtils::to_u16string(u_errorName(status)))
    , status(status)
{
}

ICUException::ICUException(UErrorCode status, const std::u16string& message)
    : super(::inflection::util::StringViewUtils::to_u16string(u_errorName(status)) + u": " + message)
    , status(status)
{
}

ICUException::~ICUException()
{
}

UErrorCode ICUException::getError() const
{
    return status;
}

} // namespace inflection::exception
