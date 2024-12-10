/*
 * Copyright 2020-2024 Apple Inc. All rights reserved.
 */
#include <morphuntion/util/VersionUtils.hpp>

#define str_quote(s) #s
#define str(s) str_quote(s)

namespace morphuntion::util {

const char* VersionUtils::getLibraryVersion()
{
    return str(MORPHUNTION_MAJOR_VERSION) "." str(MORPHUNTION_MINOR_VERSION);
}

} // namespace morphuntion::util
