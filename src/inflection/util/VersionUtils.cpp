/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include <inflection/util/VersionUtils.hpp>

#define str_quote(s) #s
#define str(s) str_quote(s)

namespace inflection::util {

const char* VersionUtils::getLibraryVersion()
{
    return str(INFLECTION_MAJOR_VERSION) "." str(INFLECTION_MINOR_VERSION);
}

} // namespace inflection::util
