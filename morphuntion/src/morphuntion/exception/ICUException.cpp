/**
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/exception/ICUException.hpp>
#include <morphuntion/util/StringViewUtils.hpp>

namespace morphuntion::exception {

ICUException::ICUException(UErrorCode status)
    : super(::morphuntion::util::StringViewUtils::to_u16string(u_errorName(status)))
{
}

ICUException::ICUException(UErrorCode status, const std::u16string& message)
    : super(::morphuntion::util::StringViewUtils::to_u16string(u_errorName(status)) + u": " + message)
{
}

ICUException::~ICUException()
{
}

} // namespace morphuntion::exception
